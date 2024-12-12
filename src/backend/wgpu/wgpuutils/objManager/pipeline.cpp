#include "pipeline.hpp"
#include <iostream>

namespace pen::backend {
    WGPURenderPipelineDescriptor create2DPipelineDescriptor(WGPUVertexBufferLayout *layout2D, WGPUShaderModule shaderModule, WGPUTextureFormat surfaceFormat) {
        WGPURenderPipelineDescriptor pipelineDesc = {};
        pipelineDesc.nextInChain = nullptr;
        // DESCRIBE RENDER PIPELINE
        // ------------------------

        // Describe vertex pipeline state
        // ------------------------------
        if (layout2D != nullptr) {
            pipelineDesc.vertex.bufferCount = 1;
	        pipelineDesc.vertex.buffers = layout2D;
        } else {
            pipelineDesc.vertex.bufferCount = 0;
	        pipelineDesc.vertex.buffers = nullptr;
            std::cout << "PENUMBRA_ERROR [WGPU]: Couldn't bind Vertex Layout for this Pipeline (no vertex layout defined before shader creation)\n";
        }
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
        pipelineDesc.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
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
        blendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
        blendState.color.operation = WGPUBlendOperation_Add;
        // Alpha blending equation
        blendState.alpha.srcFactor = WGPUBlendFactor_SrcAlpha;
        blendState.alpha.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
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
        // Uniform Layout. Here we set Uniform layouts and stuff
        pipelineDesc.layout = nullptr;

        // Return
        return pipelineDesc;
    }
}