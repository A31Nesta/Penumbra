#include "../backendIdxBuffer.hpp"
#include "backend/wgpu/wgpuutils/objectManager.hpp"

#include <vector>

namespace pen::backend {
    BackendIdxBuffer::BackendIdxBuffer(std::vector<uint16_t> idxArr) {
        this->idxArr = idxArr;

        id = createIndexBuffer(idxArr);
    }

    BackendIdxBuffer::~BackendIdxBuffer() {
        destroyIndexBuffer(id);
    }
}