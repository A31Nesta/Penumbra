#pragma once

/**
    Utility with defaults for render pipeline creation.
    It includes functions for 2D and 3D pipelines
*/

#include "webgpu.h"

namespace pen::backend {
    WGPURenderPipelineDescriptor create2DPipelineDescriptor(WGPUVertexBufferLayout* layout2D, WGPUShaderModule shaderModule, WGPUTextureFormat surfaceFormat);
}