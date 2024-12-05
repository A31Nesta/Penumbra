#include "../backendIdxBuffer.hpp"

#include <vector>

#include <bgfx/bgfx.h>

namespace pen::backend {
    BackendIdxBuffer::BackendIdxBuffer(std::vector<uint16_t> idxArr) {
        this->idxArr = idxArr;

        // Create Index Buffer
        id = bgfx::createIndexBuffer(bgfx::makeRef(this->idxArr.data(), this->idxArr.size() * sizeof(uint16_t))).idx;
    }

    BackendIdxBuffer::~BackendIdxBuffer() {
        bgfx::destroy((bgfx::IndexBufferHandle)id);
    }
}