#include "objectManager.hpp"

#include "webgpu.h"

#include <cstdint>
#include <vector>

namespace pen::backend {
    // Global Data
    namespace objects {
        WGPUDevice device;
        WGPUQueue queue;
        // Surface
        WGPUSurface surface;
        WGPUSurfaceCapabilities surfaceCapabilities;
        WGPUSurfaceConfiguration config;
        // Pipeline
        WGPUTextureFormat surfaceFormat = WGPUTextureFormat_Undefined;
    }
    
    // WGPU Pipeline register
    std::vector<WGPURenderPipeline*> renderPipelines;

    // Registers a pipeline and returns its ID
    uint16_t registerPipeline(WGPURenderPipeline* pipeline) {
        uint16_t index = -1; // Invalid ID by default

        // Check for re-usable spots
        for (uint16_t i = 0; i < renderPipelines.size(); i++) {
            WGPURenderPipeline* p = renderPipelines.at(i);
            if (p == nullptr) {
                index = i;
                break;
            }
        }

        // If the ID is valid set the pipeline and return the ID
        if (index < renderPipelines.size()) {
            renderPipelines.at(index) = pipeline;
            return index;
        } 
        // If the ID is invalid we didn't have free spots.
        // We should just add this new one
        else {
            index = renderPipelines.size();
            renderPipelines.push_back(pipeline);
            return index;
        }
    }

    // Takes a path to a shader and returns the ID of the pipeline
    uint16_t createRenderPipeline(std::string shaderCode) {
        // RENDER PIPELINE
        WGPURenderPipeline* pipeline = new WGPURenderPipeline;

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
        shaderCodeDesc.code = shaderCode.c_str();

        WGPUShaderModule shaderModule = wgpuDeviceCreateShaderModule(objects::device, &shaderDesc);

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
        colorTarget.format = objects::surfaceFormat;
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
        *pipeline = wgpuDeviceCreateRenderPipeline(objects::device, &pipelineDesc);

        // Delete shader module. 1 pipeline per shader so....
        wgpuShaderModuleRelease(shaderModule);

        // REGISTER PIPELINE AND RETURN ID
        // -------------------------------
        return registerPipeline(pipeline);
    }

    // Sets the pipeline specified
    void setRenderPipeline(WGPURenderPassEncoder renderPass, uint16_t pipeline) {
        wgpuRenderPassEncoderSetPipeline(renderPass, *renderPipelines.at(pipeline));
    }


    // Deinitializes all objects
    void deinitAllObjects() {
        // Unconfigure the surface
        wgpuSurfaceUnconfigure(objects::surface);
        // Free members
        wgpuSurfaceCapabilitiesFreeMembers(objects::surfaceCapabilities);
        // Release
        wgpuQueueRelease(objects::queue);
        wgpuSurfaceRelease(objects::surface);
        wgpuDeviceRelease(objects::device);
    }
}