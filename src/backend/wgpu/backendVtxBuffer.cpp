#include "../backendVtxBuffer.hpp"

#include "backend/backendVtxLayout.hpp"
#include "backend/wgpu/wgpuutils/objectManager.hpp"
#include <vector>

#define ANTUMBRA_FLOATS_PER_VERTEX 5 // 3 position + 2 uv

namespace pen::backend {
    BackendVtxBuffer::BackendVtxBuffer(std::vector<float> vtxArr, BackendVtxLayout& vtxLayout)
    : vtxLayout(vtxLayout) {
        this->vtxArr = vtxArr;

        id = createVertexBuffer(vtxArr);
    }

    BackendVtxBuffer::~BackendVtxBuffer() {
        destroyVertexBuffer(id);
    }
}