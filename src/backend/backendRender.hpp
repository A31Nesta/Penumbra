#pragma once

/**
    Backend-independent Render Functions
    ------------------------------------
*/

#include "shader.hpp"
#include "backendIdxBuffer.hpp"
#include "backendVtxBuffer.hpp"

#include <cstdint>

namespace pen::backend {
    // Framebuffer Stuff
    void bindFramebuffer(uint32_t framebufferID);

    // General
    void setViewTransform(float* viewMtx, float* projMtx);

    // Models
    void setModelTransform(float* modelMtx);
    void setBuffers(BackendVtxBuffer* bvb, BackendIdxBuffer* bib);

    // Draw Element
    void drawCurrent(Shader* shader);
}