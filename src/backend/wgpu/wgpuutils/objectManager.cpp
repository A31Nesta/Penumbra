#include "objectManager.hpp"

#include "backend/wgpu/wgpuutils/objManager/pipeline.hpp"
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

    // Pipeline Templates and Bind Group Layouts
    namespace pipeline2D {
        WGPUBindGroupLayout bindGroupLayout;
    }
    
    // WGPU Pipeline register
    std::vector<WGPURenderPipeline*> renderPipelines;
    std::vector<WGPUBuffer*> vertexBuffers;
    std::vector<WGPUBuffer*> indexBuffers;

    // Initializes Object Manager
    // Inits the Bind Group Layouts
    void initObjectManager() {
        
    }

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
        // We set it to default 2D Parameters
        WGPURenderPipelineDescriptor pipelineDesc = create2DPipelineDescriptor(layouts::layout2D, shaderModule, objects::surfaceFormat);
        
        // Set Layout here.
        

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