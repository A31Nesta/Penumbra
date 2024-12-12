#pragma once

/**
    Utility with defaults for render pipeline creation.
    It includes functions for 2D and 3D pipelines
*/

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_float4x4.hpp>

#include "webgpu.h"

namespace pen::backend {
    // View and Projection matrices
    struct ViewProjection {
        glm::mat4 view = glm::mat4(1);
        glm::mat4 projection = glm::mat4(1);
    };

    WGPURenderPipelineDescriptor create2DPipelineDescriptor(WGPUVertexBufferLayout* layout2D, WGPUShaderModule shaderModule, WGPUTextureFormat surfaceFormat);
    void create2DBindGroupLayouts(WGPUBindGroupLayout& layoutVP, WGPUBindGroupLayout& layoutM, WGPUDevice device);


    // Set Default WGPUBindGroupLayoutEntry
    void setDefault(WGPUBindGroupLayoutEntry &bindingLayout);
}