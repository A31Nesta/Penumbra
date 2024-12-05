#pragma once

/**
    Backend-independent Vertex Buffer
    ---------------------------------

    Creates and destroys the vertex buffer.
    The vertex buffer contains data from a mesh's
    vertices array, so it may contain things like
    UVs or normals.

*/

#include "backend/backendVtxLayout.hpp"
#include <cstdint>

namespace pen::backend {
    class BackendVtxBuffer {
    public:
        BackendVtxBuffer(std::vector<float> vtxArr, BackendVtxLayout& vtxLayout);
        ~BackendVtxBuffer();

        uint16_t getID() { return id; }

    private:
        // Obtained from the rendering backend.
        // In BGFX this is a VertexBufferHandle
        uint16_t id;

        // Vertex Array. It is always a float array
        // because the attributes are always float
        std::vector<float> vtxArr;

        // Vertex Layout
        BackendVtxLayout vtxLayout;
    };
}