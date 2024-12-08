#include "../backendWindow.hpp"
#include "utils/config.hpp"
#include "wgpu/wgpu.h"
#include "webgpu/webgpu.h"

#include <GLFW/glfw3.h>

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

// WGPU
#include "wgpuutils/init.hpp"

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif // __EMSCRIPTEN__


/**
    BGFX Implementation of Backend Window
    -------------------------------------

    All the BGFX Code related to window creation and handling goes here  
*/

namespace pen::backend {
    namespace callbacks {
        static void glfw_framebufferResizeCallback(GLFWwindow* window, int width, int height) {
            backend::BackendWindow* win = (backend::BackendWindow*)glfwGetWindowUserPointer(window);
            win->resize(width, height);
        }
    }

    // WGPU Global Variables
    // ---------------------
    WGPUDevice device;
    WGPUQueue queue;
    WGPUSurface surface;
    WGPUSurfaceCapabilities surfaceCapabilities;
    WGPUSurfaceConfiguration config;

    // WGPU Helper Variables
    // ---------------------
    WGPUTextureView getNextSurfaceTextureView() {
        WGPUSurfaceTexture surfaceTexture;
        wgpuSurfaceGetCurrentTexture(surface, &surfaceTexture);

        if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_Success) {
            return nullptr;
        }

        // Sussy magic
        WGPUTextureViewDescriptor viewDescriptor;
        viewDescriptor.nextInChain = nullptr;
        viewDescriptor.label = "Surface texture view";
        viewDescriptor.format = wgpuTextureGetFormat(surfaceTexture.texture);
        viewDescriptor.dimension = WGPUTextureViewDimension_2D;
        viewDescriptor.baseMipLevel = 0;
        viewDescriptor.mipLevelCount = 1;
        viewDescriptor.baseArrayLayer = 0;
        viewDescriptor.arrayLayerCount = 1;
        viewDescriptor.aspect = WGPUTextureAspect_All;
        WGPUTextureView targetView = wgpuTextureCreateView(surfaceTexture.texture, &viewDescriptor);

        // Apparently we shouldn't do this in WGPU-Native
        // wgpuTextureRelease(surfaceTexture.texture);

        return targetView;
    }


    // PUBLIC:

    // Shows the framebuffers used (2D and/or 3D) to the screen
    // and polls events
    void BackendWindow::update() {
        // For now we put things here and ignore the things we're actually supposed to render lol
        WGPUTextureView targetView = getNextSurfaceTextureView();
        if (!targetView) return; // A bit spaghetti because we don't update the time

        // Create Command Encoder
        WGPUCommandEncoderDescriptor encoderDescriptor = {};
        encoderDescriptor.nextInChain = nullptr;
        encoderDescriptor.label = "Command Encoder";
        WGPUCommandEncoder encoder = wgpuDeviceCreateCommandEncoder(device, &encoderDescriptor);

        // Create Render Pass Descriptor and Attachments
        WGPURenderPassDescriptor renderPassDescriptor = {};
        renderPassDescriptor.nextInChain = nullptr;

        WGPURenderPassColorAttachment renderPassColorAttachment = {};

        renderPassColorAttachment.view = targetView; // We draw directly to the window texture. Post processing passes/other framebuffers would need to change this
        renderPassColorAttachment.resolveTarget = nullptr; // A second target texture view. Apparently used for multisampling
        renderPassColorAttachment.loadOp = WGPULoadOp_Clear; // Before starting the render pass we clear the screen
        renderPassColorAttachment.storeOp = WGPUStoreOp_Store; // We can store or discard the render pass (?)
        if (penumbraFlags & PENUMBRA_TRANSPARENT) {
            renderPassColorAttachment.clearValue = WGPUColor{0, 0, 0, 0}; // Clear Color
        } else {
            renderPassColorAttachment.clearValue = WGPUColor{0.117647058824, 0.0352941176471, 0.211764705882, 1.0}; // Clear Color
        }

        // Apparently this is not supported by WGPU Native
        // renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

        renderPassDescriptor.colorAttachmentCount = 1;
        renderPassDescriptor.colorAttachments = &renderPassColorAttachment;

        renderPassDescriptor.depthStencilAttachment = nullptr; // We don't use depth or stencil
        renderPassDescriptor.timestampWrites = nullptr; // We do not measure performance

        // Create Render pass
        WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDescriptor);

        // Finish render pass
        wgpuRenderPassEncoderEnd(renderPass);
        wgpuRenderPassEncoderRelease(renderPass);

        // Encode and submit the render pass
        WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
        cmdBufferDescriptor.nextInChain = nullptr;
        cmdBufferDescriptor.label = "Command buffer";
        WGPUCommandBuffer command = wgpuCommandEncoderFinish(encoder, &cmdBufferDescriptor);
        wgpuCommandEncoderRelease(encoder);

        wgpuQueueSubmit(queue, 1, &command);
	    wgpuCommandBufferRelease(command);

        // Release the texture view
        wgpuTextureViewRelease(targetView);
        // Present the next texture of the swap chain, yay!
        #ifndef __EMSCRIPTEN__
            wgpuSurfacePresent(surface);
        #endif

        // WGPU Device Poll (AKA WGPU Device Tick in Dawn)
        wgpuDevicePoll(device, false, nullptr);

        currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
    }

    // Set Window Size.
    // Rebuilds framebuffers
    void BackendWindow::resize(uint16_t x, uint16_t y) {
        width = x;
        height = y;

        config.width = width;
        config.height = height;
        wgpuSurfaceConfigure(surface, &config);
    }

    // PRIVATE:

    void BackendWindow::initBackendSpecific() {
        // Set GLFW window resize callback
        glfwSetFramebufferSizeCallback(window, callbacks::glfw_framebufferResizeCallback);
        // TODO: Set GLFW Input Callbacks
        // Set GLFW Input callbacks

        // Create WGPU Instance
        WGPUInstance instance = createWGPUInstance();
        if (!instance) {
            throw std::runtime_error("PENUMBRA_ERROR [WGPU]: Could not initialize WebGPU!");
        }

        // Get Window Surface
        surface = getSurfaceFromGLFWWindow(window, instance);

        // Request Adapter
        WGPURequestAdapterOptions adapterOpts = {};
        adapterOpts.nextInChain = nullptr;
        adapterOpts.compatibleSurface = surface;
        WGPUAdapter adapter = requestAdapterSync(instance, &adapterOpts);

        // We have the adapter, we can release the instance
        wgpuInstanceRelease(instance);
        std::cout << "PENUMBRA [WGPU]: Got adapter: " << adapter << std::endl;

        // Show adapter info (Useful to see what GPU is being used and what graphics API is being used)
        showAdapterProperties(adapter);

        // Create Descriptor
        WGPUDeviceDescriptor deviceDesc = getDeviceDescriptor();
        // Request device
        device = requestDeviceSync(adapter, &deviceDesc);

        // Get WebGPU Queue
        queue = wgpuDeviceGetQueue(device);

        // Configure Surface
        // -----------------
        config = {};
        config.nextInChain = nullptr;

        // Swap chain size
        int currentW, currentH;
        glfwGetWindowSize(window, &currentW, &currentH);
        config.width = currentW;
        config.height = currentH;

        // Set surface format to whatever the surface uses
        surfaceCapabilities = {};
        wgpuSurfaceGetCapabilities(surface, adapter, &surfaceCapabilities);
        config.format = surfaceCapabilities.formats[0];
        // And we do not need any particular view format
        config.viewFormatCount = 0;
        config.viewFormats = nullptr;

        // Set swapchain texture as render attachment and set device
        config.usage = WGPUTextureUsage_RenderAttachment;
        config.device = device;
        
        // Get Present Mode from PenumbraFlags (FIFO, Mailbox or VSync)
        if (penumbraFlags & PENUMBRA_VSYNC) {
            std::cout << "PENUMBRA [WGPU]: Set FIFO Present Mode (VSync)\n";
            config.presentMode = WGPUPresentMode_Fifo;
        } else {
            std::cout << "PENUMBRA [WGPU]: Set Immediate Present Mode (Tearing)\n";
            config.presentMode = WGPUPresentMode_Immediate;
        }
        // TODO: Add config flag to enable Mailbox

        // Set Alpha Mode according to configuration
        if (penumbraFlags & PENUMBRA_TRANSPARENT) {
            // Check if surface supports transparency
            bool supportsTransparency = false;
            for (uint8_t i = 0; i < surfaceCapabilities.alphaModeCount; i++) {
                if (surfaceCapabilities.alphaModes[i] == WGPUCompositeAlphaMode_Premultiplied) {
                    supportsTransparency = true;
                }
            }
            // Apply transparency if supported, default otherwise
            if (supportsTransparency) {
                // You can set Premultiplied or Postmultiplied.
                // Source: https://docs.rs/wgpu/latest/wgpu/enum.CompositeAlphaMode.html
                // TODO: Investigate this further.
                config.alphaMode = WGPUCompositeAlphaMode_Premultiplied;
                std::cout << "PENUMBRA [WGPU]: Created Transparent Window with Premultiplied Alpha\n";
            } else {
                config.alphaMode = WGPUCompositeAlphaMode_Auto;
                std::cout << "PENUMBRA [WGPU]: Tried to create Transparent Window but Surface doesn't support it. Using default...\n";
            }
        } else {
            config.alphaMode = WGPUCompositeAlphaMode_Auto;
        }

        // Set the current configuration
        wgpuSurfaceConfigure(surface, &config);

        // Enable debug text.
        if (penumbraFlags & PENUMBRA_DEBUG) {
            debugging = true;
        }

        // Release the adapter at the end
        wgpuAdapterRelease(adapter);
    }

    // Deinitializes Backend
    void BackendWindow::deinitBackend() {
        // Unconfigure the surface
        wgpuSurfaceUnconfigure(surface);
        // Free members
        wgpuSurfaceCapabilitiesFreeMembers(surfaceCapabilities);
        // Release
        wgpuQueueRelease(queue);
        wgpuSurfaceRelease(surface);
        wgpuDeviceRelease(device);
    }

    // Creates a framebuffer and stores its ID (the Framebuffer is in GPU memory probably)
    void BackendWindow::createFramebuffers() {
        // Create 2D and 3D Framebuffers Here...
    }
}