#include "pipeline.hpp"
#include "webgpu.h"
#include <glm/fwd.hpp>
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

    void create2DBindGroupLayouts(WGPUBindGroupLayout& layoutVP, WGPUBindGroupLayout& layoutM, WGPUDevice device) {
        // Create the two Bind Group Layout Entries
        // ----------------------------------------
        WGPUBindGroupLayoutEntry bindGroupLayouts[2];

        // View and Projection (struct)
        // ----------------------------
        bindGroupLayouts[0] = {};
        setDefault(bindGroupLayouts[0]);
        // Binding index for VP matrices is 0
        bindGroupLayouts[0].binding = 0;
        // Only the Vertex Shader needs this
        // INFO: It may be useful to change this later on
        bindGroupLayouts[0].visibility = WGPUShaderStage_Vertex;
        // This is a uniform so we set the type to uniform
        bindGroupLayouts[0].buffer.type = WGPUBufferBindingType_Uniform;
        // Min Binding Size, it is equal to the size of the Struct (ViewProjection)
        bindGroupLayouts[0].buffer.minBindingSize = sizeof(ViewProjection);

        // Model (1 4x4 Matrix)
        // --------------------
        bindGroupLayouts[1] = {};
        setDefault(bindGroupLayouts[1]);
        // Binding index for Model matrix is 1
        bindGroupLayouts[1].binding = 1;
        // Only the Vertex Shader needs this
        // INFO: It may be useful to change this later on
        bindGroupLayouts[1].visibility = WGPUShaderStage_Vertex;
        // This is a uniform so we set the type to uniform
        bindGroupLayouts[1].buffer.type = WGPUBufferBindingType_Uniform;
        // Min Binding Size, it is equal to the size of a 4x4 matrix (I use GLM for this)
        bindGroupLayouts[1].buffer.minBindingSize = sizeof(glm::mat4);

        // Create Descriptor for the Layout
        // --------------------------------
        WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor{};
        bindGroupLayoutDescriptor.nextInChain = nullptr;
        bindGroupLayoutDescriptor.entryCount = 1; // Only 1 layout entry for the VP
        bindGroupLayoutDescriptor.entries = &bindGroupLayouts[0];

        // Create Bind Group Layout
        // ------------------------
        layoutVP = wgpuDeviceCreateBindGroupLayout(device, &bindGroupLayoutDescriptor);

        // Re-use descriptor for the other layout:
        // ---------------------------------------
        bindGroupLayoutDescriptor.entries = &bindGroupLayouts[1];
        layoutM = wgpuDeviceCreateBindGroupLayout(device, &bindGroupLayoutDescriptor);
    }


    // Set Default WGPUBindGroupLayoutEntry
    void setDefault(WGPUBindGroupLayoutEntry &bindingLayout) {
        bindingLayout.buffer.nextInChain = nullptr;
        bindingLayout.buffer.type = WGPUBufferBindingType_Undefined;
        bindingLayout.buffer.hasDynamicOffset = false;

        bindingLayout.sampler.nextInChain = nullptr;
        bindingLayout.sampler.type = WGPUSamplerBindingType_Undefined;

        bindingLayout.storageTexture.nextInChain = nullptr;
        bindingLayout.storageTexture.access = WGPUStorageTextureAccess_Undefined;
        bindingLayout.storageTexture.format = WGPUTextureFormat_Undefined;
        bindingLayout.storageTexture.viewDimension = WGPUTextureViewDimension_Undefined;

        bindingLayout.texture.nextInChain = nullptr;
        bindingLayout.texture.multisampled = false;
        bindingLayout.texture.sampleType = WGPUTextureSampleType_Undefined;
        bindingLayout.texture.viewDimension = WGPUTextureViewDimension_Undefined;
    }
}