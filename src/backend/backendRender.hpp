#pragma once

/**
    Backend-independent Render Functions
    ------------------------------------
*/

#include "shader.hpp"
#include "backendIdxBuffer.hpp"
#include "backendVtxBuffer.hpp"

#include <cstdint>

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_float4x4.hpp>

namespace pen::backend {
    enum class UniformType {
        Sampler,
        Vec4,
        Mat3,
        Mat4
    };

    // Shading and Uniforms
    uint16_t createUniform(std::string name, UniformType type);
    void deleteUniform(uint16_t uniform);

    // Framebuffer Stuff
    void bindFramebuffer(void* framebufferID);

    // Transform and Model
    void setViewTransform(glm::mat4 viewMtx, glm::mat4 projMtx);
    void setModelTransform(float* modelMtx);
    void setBuffers(BackendVtxBuffer* bvb, BackendIdxBuffer* bib);

    // Draw Element
    void drawCurrent(Shader* shader);
}