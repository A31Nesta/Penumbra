#pragma once

/**
    WGPU-specific functions to aid with the creation and destruction
    of Shaders (pipelines), textures and more.
*/

#include "webgpu.h"
#include <cstdint>
#include <string>

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


    // Deinitializes all objects
    void deinitAllObjects();
}