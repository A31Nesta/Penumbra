#include "../backendVtxBuffer.hpp"

#include "backend/backendVtxLayout.hpp"
#include <vector>

#include <bgfx/bgfx.h>

namespace pen::backend {
    BackendVtxBuffer::BackendVtxBuffer(std::vector<float> vtxArr, BackendVtxLayout& vtxLayout)
    : vtxLayout(vtxLayout) {
        this->vtxArr = vtxArr;

        // Get Vertex Layout
        bgfx::VertexLayout layout = *(bgfx::VertexLayout*)this->vtxLayout.getBackendSpecificData();

        // Create Vertex Buffer
        id = bgfx::createVertexBuffer(
            bgfx::makeRef(this->vtxArr.data(), this->vtxArr.size() * sizeof(float)),
            layout)
            .idx
        ;
    }

    BackendVtxBuffer::~BackendVtxBuffer() {
        bgfx::destroy((bgfx::VertexBufferHandle)id);
    }
}