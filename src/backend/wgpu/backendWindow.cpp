#include "../backendWindow.hpp"
#include "backend/backendRender.hpp"
#include "backend/shader.hpp"
#include "utils/config.hpp"
#include "wgpu/wgpu.h"
#include "webgpu/webgpu.h"

#include <GLFW/glfw3.h>

#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

// WGPU Objects from Object Manager
#include "wgpuutils/objectManager.hpp"
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

    // WGPU Helper Functions
    // ---------------------
    WGPUTextureView getNextSurfaceTextureView(WGPUSurface surface) {
        WGPUSurfaceTexture surfaceTexture;
        wgpuSurfaceGetCurrentTexture(surface, &surfaceTexture);

        if (surfaceTexture.status != WGPUSurfaceGetCurrentTextureStatus_Success) {
            switch (surfaceTexture.status) {
            case WGPUSurfaceGetCurrentTextureStatus_Success:
                std::cout << "Surface Texture Status: SUCCESS\n";
                break;
            case WGPUSurfaceGetCurrentTextureStatus_Timeout:
                std::cout << "Surface Texture Status: TIMEOUT\n";
                break;
            case WGPUSurfaceGetCurrentTextureStatus_Outdated:
                std::cout << "Surface Texture Status: OUTDATED\n";
                break;
            case WGPUSurfaceGetCurrentTextureStatus_Lost:
                std::cout << "Surface Texture Status: LOST\n";
                break;
            case WGPUSurfaceGetCurrentTextureStatus_OutOfMemory:
                std::cout << "Surface Texture Status: OUT OF MEMORY\n";
                break;
            case WGPUSurfaceGetCurrentTextureStatus_DeviceLost:
                std::cout << "Surface Texture Status: DEVICE LOST\n";
                break;
            case WGPUSurfaceGetCurrentTextureStatus_Force32:
                std::cout << "Surface Texture Status: FORCE32\n";
                break;
            }
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
    void updateRenderPass(RenderPassObjects& renderPassObjs) {
        // WE ONLY UPDATE IF A RENDER PASS IS BEING DONE AT THE MOMENT
        // IF NOTHING IS BEING DONE WE EXIT
        if (renderPassObjs.rendering == true) {
            // Finish render pass
            wgpuRenderPassEncoderEnd(renderPassObjs.renderPass);
            wgpuRenderPassEncoderRelease(renderPassObjs.renderPass);

            // Encode and submit the render pass
            WGPUCommandBufferDescriptor cmdBufferDescriptor = {};
            cmdBufferDescriptor.nextInChain = nullptr;
            cmdBufferDescriptor.label = "Command buffer";
            WGPUCommandBuffer command = wgpuCommandEncoderFinish(renderPassObjs.encoder, &cmdBufferDescriptor);
            wgpuCommandEncoderRelease(renderPassObjs.encoder);

            wgpuQueueSubmit(objects::queue, 1, &command);
            wgpuCommandBufferRelease(command);

            // Release the texture view
            wgpuTextureViewRelease(renderPassObjs.targetView);
            // Present the next texture of the swap chain, yay!
            #ifndef __EMSCRIPTEN__
                wgpuSurfacePresent(objects::surface);
            #endif

            // WGPU Device Poll (AKA WGPU Device Tick in Dawn)
            wgpuDevicePoll(objects::device, false, nullptr);

            // SET RENDERING TO FALSE.
            // THIS INDICATES THAT WE'VE ENDED THE FRAME
            renderPassObjs.rendering = false;
        }
    }

    // WGPU Globals
    // ------------
    // Test Shader
    // TODO: Rely on Penumbra's Shader System
    Shader* shader;

    // Render Pass Objects
    RenderPassObjects renderPassObjs;

    
    // PUBLIC:

    // Sets the 2D Framebuffer or otherwise starts the 2D Render Pass
    void BackendWindow::begin2DPass() {
        // If a render pass was being done before, end it.
        // This function does nothing if no render pass was in process
        updateRenderPass(renderPassObjs);
        // Set Rendering to True
        renderPassObjs.rendering = true;

        // We need to start this render pass
        renderPassObjs.targetView = getNextSurfaceTextureView(objects::surface);
        if (!renderPassObjs.targetView) {
            // Inform error
            std::cerr << "PENUMBRA_ERROR [WGPU]: Couldn't obtain Target View\n";
            #ifdef __linux__
                // For Linux users inform of the black magic going on with X11 under Wayland Compositors
                std::cerr << "PENUMBRA_INFO [WGPU]: If you're on Wayland, check that you're not running this program with XWayland.\n";
            #endif
            std::cerr << "PENUMBRA_INFO [WGPU]: Applying the Nuclear Option: Waiting 0.1 second and trying again...\n";

            // HACK: Freeze the thread for 0.1 seconds, re-poll events and try again
            // This should be OK because it happens 
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            // Re-try. If it fails, it is time to crash :(
            glfwPollEvents();
            renderPassObjs.targetView = getNextSurfaceTextureView(objects::surface);
            if (!renderPassObjs.targetView) {
                std::cerr << "PENUMBRA_FATAL [WGPU]: Re-tried getting Surface Texture after re-polling events but an invalid Target View was obtained.\n";
                throw std::runtime_error("PENUMBRA_ERROR [WGPU]: No Target View was obtained");
            }
        }

        // Create Command Encoder
        WGPUCommandEncoderDescriptor encoderDescriptor = {};
        encoderDescriptor.nextInChain = nullptr;
        encoderDescriptor.label = "Command Encoder";
        renderPassObjs.encoder = wgpuDeviceCreateCommandEncoder(objects::device, &encoderDescriptor);

        // Create Render Pass Descriptor and Attachments
        WGPURenderPassDescriptor renderPassDescriptor = {};
        renderPassDescriptor.nextInChain = nullptr;

        WGPURenderPassColorAttachment renderPassColorAttachment = {};

        renderPassColorAttachment.view = renderPassObjs.targetView; // We draw directly to the window texture. Post processing passes/other framebuffers would need to change this
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

        renderPassDescriptor.depthStencilAttachment = nullptr; // TODO: Enable depth for Umbra Renderer Render Pass
        renderPassDescriptor.timestampWrites = nullptr; // We do not measure performance

        // Create Render pass
        renderPassObjs.renderPass = wgpuCommandEncoderBeginRenderPass(renderPassObjs.encoder, &renderPassDescriptor);

        // Bind Framebuffer in this case enables the Backend Render Functions
        // To access this render pass.
        bindFramebuffer(reinterpret_cast<void*>(&renderPassObjs));
    }
    // Sets the 3D Render Pass
    void BackendWindow::begin3DPass() {

    }

    // Shows the framebuffers used (2D and/or 3D) to the screen
    // and polls events
    void BackendWindow::update() {
        // Update last render pass being done
        updateRenderPass(renderPassObjs);

        currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
    }

    // Set Window Size.
    // Reconfigures the Surface
    void BackendWindow::resize(uint16_t x, uint16_t y) {
        width = x;
        height = y;

        objects::config.width = width;
        objects::config.height = height;
        wgpuSurfaceConfigure(objects::surface, &objects::config);
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
        objects::surface = getSurfaceFromGLFWWindow(window, instance);

        // Request Adapter
        WGPURequestAdapterOptions adapterOpts = {};
        adapterOpts.nextInChain = nullptr;
        adapterOpts.compatibleSurface = objects::surface;
        WGPUAdapter adapter = requestAdapterSync(instance, &adapterOpts);

        // We have the adapter, we can release the instance
        wgpuInstanceRelease(instance);
        std::cout << "PENUMBRA [WGPU]: Got adapter: " << adapter << std::endl;

        // Show adapter info (Useful to see what GPU is being used and what graphics API is being used)
        showAdapterProperties(adapter);

        // Create Descriptor
        WGPUDeviceDescriptor deviceDesc = getDeviceDescriptor();
        // Request device
        objects::device = requestDeviceSync(adapter, &deviceDesc);

        // Get WebGPU Queue
        objects::queue = wgpuDeviceGetQueue(objects::device);

        // Configure Surface
        // -----------------
        objects::config = {};
        objects::config.nextInChain = nullptr;

        // Swap chain size
        int currentW, currentH;
        glfwGetWindowSize(window, &currentW, &currentH);
        objects::config.width = static_cast<uint32_t>(currentW);
        objects::config.height = static_cast<uint32_t>(currentH);

        // Set surface format to whatever the surface uses
        objects::surfaceCapabilities = {};
        wgpuSurfaceGetCapabilities(objects::surface, adapter, &objects::surfaceCapabilities);
        objects::surfaceFormat = objects::surfaceCapabilities.formats[0];
        objects::config.format = objects::surfaceFormat;
        // And we do not need any particular view format
        objects::config.viewFormatCount = 0;
        objects::config.viewFormats = nullptr;

        // Set swapchain texture as render attachment and set device
        objects::config.usage = WGPUTextureUsage_RenderAttachment;
        objects::config.device = objects::device;
        
        // Get Present Mode from PenumbraFlags (FIFO, Mailbox or VSync)
        if (penumbraFlags & PENUMBRA_VSYNC) {
            std::cout << "PENUMBRA [WGPU]: Set FIFO Present Mode (VSync)\n";
            objects::config.presentMode = WGPUPresentMode_Fifo;
        } else {
            std::cout << "PENUMBRA [WGPU]: Set Immediate Present Mode (Tearing)\n";
            objects::config.presentMode = WGPUPresentMode_Immediate;
        }
        // TODO: Add config flag to enable Mailbox

        // Set Alpha Mode according to configuration
        if (penumbraFlags & PENUMBRA_TRANSPARENT) {
            // Check if surface supports transparency
            bool supportsTransparency = false;
            for (uint8_t i = 0; i < objects::surfaceCapabilities.alphaModeCount; i++) {
                if (objects::surfaceCapabilities.alphaModes[i] == WGPUCompositeAlphaMode_Premultiplied) {
                    supportsTransparency = true;
                }
            }
            // Apply transparency if supported, default otherwise
            if (supportsTransparency) {
                // You can set Premultiplied or Postmultiplied.
                // Source: https://docs.rs/wgpu/latest/wgpu/enum.CompositeAlphaMode.html
                // TODO: Investigate this further.
                objects::config.alphaMode = WGPUCompositeAlphaMode_Premultiplied;
                std::cout << "PENUMBRA [WGPU]: Created Transparent Window with Premultiplied Alpha\n";
            } else {
                objects::config.alphaMode = WGPUCompositeAlphaMode_Auto;
                std::cout << "PENUMBRA [WGPU]: Tried to create Transparent Window but Surface doesn't support it. Using default...\n";
            }
        } else {
            objects::config.alphaMode = WGPUCompositeAlphaMode_Auto;
        }

        // Set the current configuration
        wgpuSurfaceConfigure(objects::surface, &objects::config);

        // Enable debug text.
        if (penumbraFlags & PENUMBRA_DEBUG) {
            debugging = true;
        }

        // Release the adapter at the end
        wgpuAdapterRelease(adapter);

        // Test Shaders
        shader = new Shader(10, "pnmbr/shaders-2D/default");
    }

    // Deinitializes Backend
    void BackendWindow::deinitBackend() {
        // Release pipeline
        // TODO: Move to shader class
        // wgpuRenderPipelineRelease(pipeline);
        
        deinitAllObjects();
    }

    // Creates a framebuffer and stores its ID (the Framebuffer is in GPU memory probably)
    void BackendWindow::createFramebuffers() {
        // Create 2D and 3D Framebuffers Here...
    }
}