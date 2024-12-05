#pragma once

/**
    Backend-independent Render Functions
    ------------------------------------
*/

#include "backend/backendIdxBuffer.hpp"
#include "backend/backendVtxBuffer.hpp"

#include <cstdint>

namespace pen::backend {
    // General
    void setViewTransform(uint32_t framebuffer, float* viewMtx, float* projMtx);

    // Models
    void setModelTransform(float* modelMtx);
    void setBuffers(BackendVtxBuffer* bvb, BackendIdxBuffer* bib);
}