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
    // Surface
    WGPUSurface surface;
    WGPUSurfaceCapabilities surfaceCapabilities;
    WGPUSurfaceConfiguration config;
    // Pipeline
    WGPUTextureFormat surfaceFormat = WGPUTextureFormat_Undefined;
    WGPURenderPipeline pipeline;

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

    // TODO: Move this to shader code
    void InitPipeline() {
        // Create Shader module
        WGPUShaderModuleDescriptor shaderDesc = {};
        shaderDesc.hintCount = 0;
        shaderDesc.hints = nullptr;

        // Shader Code Module Descriptor
        WGPUShaderModuleWGSLDescriptor shaderCodeDesc = {}; // We use WGSL for shaders. We could use SPIR-V or GLSL though
        shaderCodeDesc.chain.next = nullptr;
        shaderCodeDesc.chain.sType = WGPUSType_ShaderModuleWGSLDescriptor;
        // Connect Chain
        shaderDesc.nextInChain = &shaderCodeDesc.chain;
        // Set Shader Source
        shaderCodeDesc.code = R"(
            @vertex
            fn vs_main(@builtin(vertex_index) in_vertex_index: u32) -> @builtin(position) vec4f {
                var p = vec2f(0.0, 0.0);
                if (in_vertex_index == 0u) {
                    p = vec2f(-0.5, -0.5);
                } else if (in_vertex_index == 1u) {
                    p = vec2f(0.5, -0.5);
                } else {
                    p = vec2f(0.0, 0.5);
                }
                return vec4f(p, 0.0, 1.0);
            }

            @fragment
            fn fs_main() -> @location(0) vec4f {
                return vec4f(1.0, 1.0, 1.0, 1.0);
            }
        )";

        WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(device, &shaderDesc);

        // Init render pipeline
        WGPURenderPipelineDescriptor pipelineDesc = {};
        pipelineDesc.nextInChain = nullptr;
        // DESCRIBE RENDER PIPELINE
        // ------------------------

        // Describe vertex pipeline state
        // ------------------------------
        pipelineDesc.vertex.bufferCount = 0; // TODO: Enable vertex buffer things amogus sussy
        pipelineDesc.vertex.buffers = nullptr;
        // NB: We define the 'shaderModule' in the second part of this chapter.
        // Here we tell that the programmable vertex shader stage is described
        // by the function called 'vs_main' in that module.
        pipelineDesc.vertex.module = shaderModule;
        pipelineDesc.vertex.entryPoint = "vs_main";
        pipelineDesc.vertex.constantCount = 0;
        pipelineDesc.vertex.constants = nullptr;

        // Describe primitive pipeline state
        // ---------------------------------
        // Each sequence of 3 vertices is considered as a triangle
        pipelineDesc.primitive.topology = WGPUPrimitiveTopology_TriangleList;
        // We'll see later how to specify the order in which vertices should be
        // connected. When not specified, vertices are considered sequentially.
        pipelineDesc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined; // TODO: Enable index buffer amogus thing
        // The face orientation is defined by assuming that when looking
        // from the front of the face, its corner vertices are enumerated
        // in the counter-clockwise (CCW) order.
        pipelineDesc.primitive.frontFace = WGPUFrontFace_CCW;
        // But the face orientation does not matter much because we do not
        // cull (i.e. "hide") the faces pointing away from us (which is often
        // used for optimization).
        pipelineDesc.primitive.cullMode = WGPUCullMode_None; // TODO: Cull back faces once everything is working (Set to Front)

        // Describe fragment pipeline state
        // --------------------------------
        // We tell that the programmable fragment shader stage is described
        // by the function called 'fs_main' in the shader module.
        WGPUFragmentState fragmentState = {};
        fragmentState.module = shaderModule;
        fragmentState.entryPoint = "fs_main";
        fragmentState.constantCount = 0;
        fragmentState.constants = nullptr;
        
        // Blending
        WGPUBlendState blendState = {};
        // Color blending equation
        blendState.color.srcFactor = WGPUBlendFactor_SrcAlpha;
        blendState.color.dstFactor = WGPUBlendFactor_OneMinusDstAlpha;
        blendState.color.operation = WGPUBlendOperation_Add;
        // Alpha blending equation
        blendState.alpha.srcFactor = WGPUBlendFactor_Zero;
        blendState.alpha.dstFactor = WGPUBlendFactor_One;
        blendState.alpha.operation = WGPUBlendOperation_Add;

        WGPUColorTargetState colorTarget = {};
        colorTarget.format = surfaceFormat;
        colorTarget.blend = &blendState;
        colorTarget.writeMask = WGPUColorWriteMask_All; // We could write to only some of the color channels.
       
        // We have only one target because our render pass has only one output color
        // attachment.
        fragmentState.targetCount = 1;
        fragmentState.targets = &colorTarget;

        // Set fragment
        pipelineDesc.fragment = &fragmentState;

        // Describe stencil/depth pipeline state
        // -------------------------------------
        // Don't use depth for now
        // TODO: Enable depth for Umbra Renderer Shaders
        pipelineDesc.depthStencil = nullptr;

        // Describe multi-sampling state
        // -----------------------------
        // No multisampling. Sample per pixel = 1
        // Samples per pixel
        pipelineDesc.multisample.count = 1;
        // Default value for the mask, meaning "all bits on"
        pipelineDesc.multisample.mask = ~0u;
        // Default value as well (irrelevant for count = 1 anyways)
        pipelineDesc.multisample.alphaToCoverageEnabled = false;

        // Describe pipeline layout
        // ------------------------
        // No memory layout for this pipeline
        // TODO: Memory layout for pipeline lol
        pipelineDesc.layout = nullptr;

        // Create render pipeline
        pipeline = wgpuDeviceCreateRenderPipeline(device, &pipelineDesc);

        // Delete shader module. 1 pipeline per shader so....
        wgpuShaderModuleRelease(shaderModule);
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

        renderPassDescriptor.depthStencilAttachment = nullptr; // TODO: Enable depth for Umbra Renderer Render Pass
        renderPassDescriptor.timestampWrites = nullptr; // We do not measure performance

        // Create Render pass
        WGPURenderPassEncoder renderPass = wgpuCommandEncoderBeginRenderPass(encoder, &renderPassDescriptor);

        // Draw triangles to this render pass
        // 1. Set Pipeline
        // 2. Draw triangle
        wgpuRenderPassEncoderSetPipeline(renderPass, pipeline);
        wgpuRenderPassEncoderDraw(renderPass, 3, 1, 0, 0);

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
    // Reconfigures the Surface
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
        surfaceFormat = surfaceCapabilities.formats[0];
        config.format = surfaceFormat;
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

        // Init pipeline.
        // This should be moved to the Shader Class later on
        InitPipeline();
    }

    // Deinitializes Backend
    void BackendWindow::deinitBackend() {
        // Release pipeline
        wgpuRenderPipelineRelease(pipeline);
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