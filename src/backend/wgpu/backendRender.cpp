#include "../backendRender.hpp"

#include "backend/wgpu/wgpuutils/objManager/pipeline.hpp"
#include "backend/wgpu/wgpuutils/objectManager.hpp"
#include "webgpu.h"
#include <glm/fwd.hpp>

namespace pen::backend {
    // Global variables
    RenderPassObjects framebuffer; // For consistency we call it framebuffer but it's actually the render pass
    uint32_t currentIndexCount = 0; // How many indices from the index buffer do we have to draw now?

    // Smol smol optimizations
    uint16_t currentPipeline = -1; // Stores the current render pipeline being used. We only change the render pipeline it's not the current one

    // Current View and Projection
    ViewProjection viewProjMatrices;

    // FUNCTIONS
    // ---------

    // Shading and Uniforms
    // This is not needed in WebGPU
    uint16_t createUniform([[maybe_unused]] std::string name, [[maybe_unused]] UniformType type) {
        return 0;
    }
    // We didn't need to create anything so...
    void deleteUniform([[maybe_unused]] uint16_t uniform) {
        // Do nothing
    }
    // USED IN SPRITE CREATION IN WGPU
    uint16_t createSpriteUniform([[maybe_unused]] std::string name, [[maybe_unused]] UniformType type) {
        // When the Mat4 is sus
        if (type == UniformType::Mat4) {
            return createUniformData(sizeof(glm::mat4), 1, name);
        }
        return 0;
    }
    void deleteSpriteUniform([[maybe_unused]] uint16_t uniform) {
        destroyUniformData(uniform);
    }

    void bindTexture(uint16_t texture) {
        bindTexture(framebuffer.renderPass, texture);
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
    void setViewTransform(glm::mat4 viewMtx, glm::mat4 projMtx) {
        wgpuRenderPassEncoderSetBindGroup(framebuffer.renderPass, 0, objects::viewProjection.bindGroup, 0, nullptr);
        viewProjMatrices.view = viewMtx;
        viewProjMatrices.projection = projMtx;
        wgpuQueueWriteBuffer(objects::queue, objects::viewProjection.uniformBuffer, 0, &viewProjMatrices, sizeof(ViewProjection));
    }
    void setModelTransform(antumbra::Sprite* sprite) {
        setBindGroup(framebuffer.renderPass, sprite->_getUniformData(), sprite->transform.getMatrix(), sizeof(glm::mat4));
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