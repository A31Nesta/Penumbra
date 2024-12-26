#pragma once

/**
    Backend-independent Render Functions
    ------------------------------------
*/

#include "antumbra/types/sprite.hpp"
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
    // INFO: [[maybe_unused]] is used here because of differences between backends. WGPU needs bind groups for each sprite, BGFX creates the uniform once
    // TODO: Remove createUniform and manage this piece of BGFX code from an Object Manager-ish function
    uint16_t createUniform([[maybe_unused]] std::string name, [[maybe_unused]] UniformType type);
    void deleteUniform([[maybe_unused]] uint16_t uniform);
    uint16_t createSpriteUniform([[maybe_unused]] std::string name, [[maybe_unused]] UniformType type);
    void deleteSpriteUniform([[maybe_unused]] uint16_t uniform);

    void bindTexture(uint16_t texture);

    // Framebuffer Stuff
    void bindFramebuffer(void* framebufferID);

    // Transform and Model
    void setViewTransform(glm::mat4 viewMtx, glm::mat4 projMtx);
    void setModelTransform(antumbra::Sprite* sprite);
    void setBuffers(BackendVtxBuffer* bvb, BackendIdxBuffer* bib);

    // Draw Element
    void drawCurrent(Shader* shader);
}