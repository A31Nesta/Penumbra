#pragma once

/**
    Backend-independent Index Buffer
    --------------------------------

    Creates and destroys the vertex buffer.
    The vertex buffer contains data from a mesh's
    vertices array, so it may contain things like
    UVs or normals.

*/

#include <cstdint>
#include <vector>

namespace pen::backend {
    class BackendIdxBuffer {
    public:
        BackendIdxBuffer(std::vector<uint16_t> idxArr);
        ~BackendIdxBuffer();

        uint16_t getID() { return id; }

    private:
        // Obtained from the rendering backend.
        // In BGFX this is a IndexBufferHandle
        uint16_t id;

        // Index Array. It is always a uint16_t array
        std::vector<uint16_t> idxArr;
    };
}