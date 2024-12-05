#include "../backendIdxBuffer.hpp"

#include <vector>

namespace pen::backend {
    BackendIdxBuffer::BackendIdxBuffer(std::vector<uint16_t> idxArr) {
        this->idxArr = idxArr;

        // Create Index Buffer
    }

    BackendIdxBuffer::~BackendIdxBuffer() {
        // Delete Index Buffer
    }
}