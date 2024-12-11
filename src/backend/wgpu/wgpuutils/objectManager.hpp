#pragma once

/**
    WGPU-specific functions to aid with the creation and destruction
    of Shaders (pipelines), textures and more.
*/

#include "webgpu.h"
#include <cstdint>
#include <string>
#include <vector>

namespace pen::backend {
    namespace objects {
        extern WGPUDevice device;
        extern WGPUQueue queue;
        // Surface
        extern WGPUSurface surface;
        extern WGPUSurfaceCapabilities surfaceCapabilities;
        extern WGPUSurfaceConfiguration config;
        // Pipeline
        extern WGPUTextureFormat surfaceFormat;
    }
    namespace layouts {
        // We specify that this layout is used for 2D Only
        extern WGPUVertexBufferLayout* layout2D;
    }

    // Structs
    struct RenderPassObjects {
        WGPUTextureView targetView;
        WGPUCommandEncoder encoder;
        WGPURenderPassEncoder renderPass;

        bool rendering = false;
    };


    // Takes a path to a shader and returns the ID of the pipeline
    uint16_t createRenderPipeline(std::string shaderCode);
    // Sets the pipeline specified
    void setRenderPipeline(WGPURenderPassEncoder renderPass, uint16_t pipeline);
    // Destroy render pipeline
    void destroyRenderPipeline(uint16_t pipeline);

    // Creates Vertex Buffer from a vector of Vertices
    uint16_t createVertexBuffer(std::vector<float> vtxArr);
    // Destroy Vertex Buffer
    void destroyVertexBuffer(uint16_t buffer);

    // Creates Index Buffer from a vector of Indices
    uint16_t createIndexBuffer(std::vector<uint16_t> idxArr);
    // Destroy Index Buffer
    void destroyIndexBuffer(uint16_t buffer);

    // Sets the Vertex and Index Buffers
    void setBuffers(WGPURenderPassEncoder& renderPass, uint16_t vertexBuffer, uint16_t indexBuffer);


    // Deinitializes all objects
    void deinitAllObjects();
}