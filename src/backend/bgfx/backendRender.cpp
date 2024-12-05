#include "../backendRender.hpp"

#include <bgfx/bgfx.h>

namespace pen::backend {
    // General
    void setViewTransform(uint32_t framebuffer, float* viewMtx, float* projMtx) {
        bgfx::setViewTransform(framebuffer, viewMtx, projMtx);

        // for (uint16_t i = 0; i < 16; i++) {
        //     std::cout << viewMtx[i] << " ";
        // }
        // std::cout << "\n";

        // for (uint16_t i = 0; i < 16; i++) {
        //     std::cout << projMtx[i] << " ";
        // }
        // std::cout << "\n\n";
    }

    // Models
    void setModelTransform(float* modelMtx) {
        bgfx::setTransform(modelMtx);
    }
    void setBuffers(BackendVtxBuffer* bvb, BackendIdxBuffer* bib) {
        uint16_t vbhID = bvb->getID();
        uint16_t ibhID = bib->getID();
        bgfx::setVertexBuffer(0, (bgfx::VertexBufferHandle)vbhID);
        bgfx::setIndexBuffer((bgfx::IndexBufferHandle)ibhID);
    }
}