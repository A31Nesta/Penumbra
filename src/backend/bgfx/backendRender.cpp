#include "../backendRender.hpp"

#include <bgfx/bgfx.h>

namespace pen::backend {
    // Global variables
    uint32_t framebuffer = 0; // In this case it is the view ID

    // Framebuffer Stuff
    void bindFramebuffer(uint32_t framebufferID) {
        framebuffer = framebufferID;
    }

    // General
    void setViewTransform(float* viewMtx, float* projMtx) {
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

    // Draw Element
    void drawCurrent(Shader* shader) {
        // Set render state. Default without Z and with Alpha
        // Disabling Z makes alpha work properly
        bgfx::setState(BGFX_STATE_DEFAULT // Use default
            ^ BGFX_STATE_WRITE_Z // Remove Z
            | BGFX_STATE_BLEND_ALPHA // Enable Alpha
        );

        // Draw
        bgfx::submit(framebuffer, (bgfx::ProgramHandle)shader->getProgram());
    }
}