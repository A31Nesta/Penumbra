#include "../backendRender.hpp"

#include "backend/wgpu/wgpuutils/objectManager.hpp"
#include "webgpu.h"

namespace pen::backend {
    // Global variables
    RenderPassObjects framebuffer; // For consistency we call it framebuffer but it's actually the render pass
    uint32_t currentIndexCount = 0; // How many indices from the index buffer do we have to draw now?

    // Smol smol optimizations
    uint16_t currentPipeline = -1; // Stores the current render pipeline being used. We only change the render pipeline it's not the current one

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
        // Sets the current Render Pass Encoder
        framebuffer = *reinterpret_cast<RenderPassObjects*>(framebufferID);

        // Important: Reset currentPipeline to -1.
        // If we don't do this we could end up never binding the pipeline
        // to the render pass
        currentPipeline = -1;
    }

    // Transform and Model
    void setViewTransform(float* viewMtx, float* projMtx) {
        // Set View and Projection matrices
    }
    void setModelTransform(float* modelMtx) {
        // Set Model Transform matrix
    }
    void setBuffers(BackendVtxBuffer* bvb, BackendIdxBuffer* bib) {
        setBuffers(framebuffer.renderPass, bvb->getID(), bib->getID());
        currentIndexCount = bib->getSize();
    }

    // Draw Element
    void drawCurrent(Shader* shader) {
        // Are we using another pipeline?
        if (currentPipeline != shader->getProgram()) {
            // If we are, we need to set the current one.
            setRenderPipeline(framebuffer.renderPass, shader->getProgram());
            // Save this pipeline as current
            currentPipeline = shader->getProgram();
        }
        // Draw the indices of the quad now that we know that we're using the correct pipeline
        wgpuRenderPassEncoderDrawIndexed(framebuffer.renderPass, currentIndexCount, 1, 0, 0, 0);
    }
}