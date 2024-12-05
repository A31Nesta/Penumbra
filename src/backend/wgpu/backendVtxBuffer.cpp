#include "../backendVtxBuffer.hpp"

#include "backend/backendVtxLayout.hpp"
#include <vector>

namespace pen::backend {
    BackendVtxBuffer::BackendVtxBuffer(std::vector<float> vtxArr, BackendVtxLayout& vtxLayout)
    : vtxLayout(vtxLayout) {
        this->vtxArr = vtxArr;

        // Create Vertex Buffer
    }

    BackendVtxBuffer::~BackendVtxBuffer() {
        // Delete Vertex handle here
    }
}