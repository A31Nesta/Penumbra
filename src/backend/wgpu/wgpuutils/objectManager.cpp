#include "objectManager.hpp"

#include "webgpu.h"

#include <cstdint>
#include <iostream>
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
    namespace layouts {
        // We specify that this layout is used for 2D Only
        WGPUVertexBufferLayout* layout2D;
    }
    
    // WGPU Pipeline register
    std::vector<WGPURenderPipeline*> renderPipelines;
    std::vector<WGPUBuffer*> vertexBuffers;
    std::vector<WGPUBuffer*> indexBuffers;

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
    // Registers a vertex buffer and returns its ID
    uint16_t registerVertexBuffer(WGPUBuffer* buffer) {
        uint16_t index = -1; // Invalid ID by default

        // Check for re-usable spots
        for (uint16_t i = 0; i < vertexBuffers.size(); i++) {
            WGPUBuffer* buf = vertexBuffers.at(i);
            if (buf == nullptr) {
                index = i;
                break;
            }
        }

        // If the ID is valid set the buffer and return the ID
        if (index < vertexBuffers.size()) {
            vertexBuffers.at(index) = buffer;
            return index;
        } 
        // If the ID is invalid we didn't have free spots.
        // We should just add this new one
        else {
            index = vertexBuffers.size();
            vertexBuffers.push_back(buffer);
            return index;
        }
    }
    // Registers an index buffer and returns its ID
    uint16_t registerIndexBuffer(WGPUBuffer* buffer) {
        uint16_t index = -1; // Invalid ID by default

        // Check for re-usable spots
        for (uint16_t i = 0; i < indexBuffers.size(); i++) {
            WGPUBuffer* buf = indexBuffers.at(i);
            if (buf == nullptr) {
                index = i;
                break;
            }
        }

        // If the ID is valid set the buffer and return the ID
        if (index < indexBuffers.size()) {
            indexBuffers.at(index) = buffer;
            return index;
        } 
        // If the ID is invalid we didn't have free spots.
        // We should just add this new one
        else {
            index = indexBuffers.size();
            indexBuffers.push_back(buffer);
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
        if (layouts::layout2D != nullptr) {
            pipelineDesc.vertex.bufferCount = 1;
	        pipelineDesc.vertex.buffers = layouts::layout2D;
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
        blendState.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
        blendState.color.operation = WGPUBlendOperation_Add;
        // Alpha blending equation
        blendState.alpha.srcFactor = WGPUBlendFactor_SrcAlpha;
        blendState.alpha.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
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
    // Destroy render pipeline
    void destroyRenderPipeline(uint16_t pipeline) {
        wgpuRenderPipelineRelease(*renderPipelines.at(pipeline));
        delete renderPipelines.at(pipeline);
        renderPipelines.at(pipeline) = nullptr;
    }

    // Creates Vertex Buffer from a vector of Vertices
    uint16_t createVertexBuffer(std::vector<float> vtxArr) {
        WGPUBuffer* vtxBuffer = new WGPUBuffer;

        // Descriptor
        WGPUBufferDescriptor bufferDescriptor{};
        bufferDescriptor.nextInChain = nullptr;
        bufferDescriptor.size = vtxArr.size() * sizeof(float);
        bufferDescriptor.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex; // Copy from CPU to GPU + It's a Vertex Buffer
        bufferDescriptor.mappedAtCreation = false;
        
        *vtxBuffer = wgpuDeviceCreateBuffer(objects::device, &bufferDescriptor);

        // Send buffer to GPU Memory
        wgpuQueueWriteBuffer(objects::queue, *vtxBuffer, 0, vtxArr.data(), bufferDescriptor.size);

        // REGISTER VERTEX BUFFER AND RETURN ID
        // ------------------------------------
        return registerVertexBuffer(vtxBuffer);
    }
    // Destroy Vertex Buffer
    void destroyVertexBuffer(uint16_t buffer) {
        // I prefer having full control over when the object is released
        // With buffers I can call Destroy instead of Release to destroy the object now
        wgpuBufferDestroy(*vertexBuffers.at(buffer));
        delete vertexBuffers.at(buffer);
        vertexBuffers.at(buffer) = nullptr;
    }

    // Creates Index Buffer from a vector of Indices
    uint16_t createIndexBuffer(std::vector<uint16_t> idxArr) {
        WGPUBuffer* idxBuffer = new WGPUBuffer;

        // Descriptor
        WGPUBufferDescriptor bufferDescriptor{};
        bufferDescriptor.nextInChain = nullptr;
        bufferDescriptor.size = idxArr.size() * sizeof(uint16_t);
        bufferDescriptor.size = (bufferDescriptor.size + 3) & ~3; // round up to the next multiple of 4
        bufferDescriptor.usage = WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index; // Copy from CPU to GPU + It's a Index Buffer
        bufferDescriptor.mappedAtCreation = false;
        
        *idxBuffer = wgpuDeviceCreateBuffer(objects::device, &bufferDescriptor);

        // Write buffer to GPU Memory
        wgpuQueueWriteBuffer(objects::queue, *idxBuffer, 0, idxArr.data(), bufferDescriptor.size);

        // REGISTER INDEX BUFFER AND RETURN ID
        // ------------------------------------
        return registerIndexBuffer(idxBuffer);
    }
    // Destroy Index Buffer
    void destroyIndexBuffer(uint16_t buffer) {

    }

    // Sets the Vertex and Index Buffers
    void setBuffers(WGPURenderPassEncoder& renderPass, uint16_t vertexBuffer, uint16_t indexBuffer) {
        WGPUBuffer vtxBuf = *vertexBuffers.at(vertexBuffer);
        WGPUBuffer idxBuf = *indexBuffers.at(indexBuffer);

        wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, vtxBuf, 0, wgpuBufferGetSize(vtxBuf));
        wgpuRenderPassEncoderSetIndexBuffer(renderPass, idxBuf, WGPUIndexFormat_Uint16, 0, wgpuBufferGetSize(idxBuf));
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