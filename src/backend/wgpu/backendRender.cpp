#include "../backendRender.hpp"

#include "backend/wgpu/wgpuutils/objectManager.hpp"
#include "webgpu.h"

namespace pen::backend {
    // Global variables
    RenderPassObjects framebuffer; // For consistency we call it framebuffer but it's actually the render pass
    uint32_t currentIndexCount = 0; // How many indices from the index buffer do we have to draw now?

    // FUNCTIONS
    // ---------

    // Shading and Uniforms
    uint16_t createUniform(std::string name, UniformType type) {
        // Implement here...
        return 0;
    }
    void deleteUniform(uint16_t uniform) {
        // Delete uniform...
    }

    // Framebuffer Stuff
    void bindFramebuffer(void* framebufferID) {
        framebuffer = *reinterpret_cast<RenderPassObjects*>(framebufferID);
    }

    // General
    void setViewTransform(float* viewMtx, float* projMtx) {
        // Set View and Projection matrices
    }

    // Models
    void setModelTransform(float* modelMtx) {
        // Set Model Transform matrix
    }
    void setBuffers(BackendVtxBuffer* bvb, BackendIdxBuffer* bib) {
        setBuffers(framebuffer.renderPass, bvb->getID(), bib->getID());
        currentIndexCount = bib->getSize();
    }

    // Draw Element
    void drawCurrent(Shader* shader) {
        // Some code to draw the currently bound element...
        setRenderPipeline(framebuffer.renderPass, shader->getProgram());
        wgpuRenderPassEncoderDrawIndexed(framebuffer.renderPass, currentIndexCount, 1, 0, 0, 0);
    }
}