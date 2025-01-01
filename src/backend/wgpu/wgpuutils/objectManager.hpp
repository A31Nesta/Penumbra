#pragma once

/**
    WGPU-specific functions to aid with the creation and destruction
    of Shaders (pipelines), textures and more.
*/

#include <webgpu-headers/webgpu.h>
#include <cstdint>
#include <string>
#include <vector>

namespace pen::backend {
    // Structs
    struct RenderPassObjects {
        WGPUTextureView targetView;
        WGPUCommandEncoder encoder;
        WGPURenderPassEncoder renderPass;

        bool rendering = false;
    };

    // Sprite and Model
    // Bind Group and Buffer all in one :D
    struct UniformData {
        WGPUBuffer uniformBuffer;
        WGPUBindGroup bindGroup;
        uint16_t groupIndex;
    };

    // Texture data
    // Texture and Texture View
    struct TextureData {
        WGPUTexture texture;
        WGPUTextureView textureView;
        WGPUSampler sampler;
        // In Antumbra (2D) we use a new Bind Group per Texture
        WGPUBindGroup textureBindGroup;
    };


    // Global stuff
    namespace objects {
        extern WGPUDevice device;
        extern WGPUQueue queue;
        // Surface
        extern WGPUSurface surface;
        extern WGPUSurfaceCapabilities surfaceCapabilities;
        extern WGPUSurfaceConfiguration config;
        // Pipeline
        extern WGPUTextureFormat surfaceFormat;

        // View and Projection Uniforms (general)
        extern UniformData viewProjection;
    }
    namespace layouts {
        // We specify that this layout is used for 2D Only
        extern WGPUVertexBufferLayout* layout2D;
    }

    // Pipeline Templates and Bind Group Layouts
    namespace pipeline2D {
        // 0 - View and Perspective
        // 1 - Model
        // 2 - Texture
        extern WGPUBindGroupLayout bindGroupLayouts[3];
    }

    // Inits the Bind Group Layouts, duh
    void initBindGroupLayouts();
    // Inits the Bind Group for the View and Projection
    void initViewProjectionUniform();


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

    // Creates a Texture and Texture View
    uint16_t createTexture(std::vector<uint8_t> data, uint16_t w, uint16_t h);
    // Destroys the texture and texture view
    void destroyTexture(uint16_t texture);


    // UNIFORMS
    // Creates a UniformData object (bind group and buffer)
    uint16_t createUniformData(uint64_t bufferSize, uint16_t bindingLayoutID, std::string label);
    // Destroys the Uniform Data
    void destroyUniformData(uint16_t uniformData);



    // Sets the Vertex and Index Buffers
    void setBuffers(WGPURenderPassEncoder& renderPass, uint16_t vertexBuffer, uint16_t indexBuffer);
    // Sets a Bind Group and writes to buffer
    void setBindGroup(WGPURenderPassEncoder& renderPass, uint16_t uniform, void* data, uint64_t sizeofData);
    // Binds a texture
    void bindTexture(WGPURenderPassEncoder& renderPass, uint16_t texture);


    // Deinitializes all objects
    void deinitAllObjects();
}