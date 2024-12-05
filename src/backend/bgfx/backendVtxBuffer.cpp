#include "../backendVtxBuffer.hpp"

#include "backend/backendVtxLayout.hpp"
#include <vector>

#include <bgfx/bgfx.h>

namespace pen::backend {
    BackendVtxBuffer::BackendVtxBuffer(std::vector<float> vtxArr, BackendVtxLayout& vtxLayout)
    : vtxLayout(vtxLayout) {
        this->vtxArr = vtxArr;

        // Create Vertex Buffer
        id = bgfx::createVertexBuffer(
            bgfx::makeRef(vtxArr.data(), vtxArr.size() * sizeof(float)),
            *(bgfx::VertexLayout*)this->vtxLayout.getBackendSpecificData())
            .idx
        ;
    }

    BackendVtxBuffer::~BackendVtxBuffer() {
        bgfx::destroy((bgfx::VertexBufferHandle)id);
    }
}