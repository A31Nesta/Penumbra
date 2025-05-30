#include "../backendRender.hpp"

#include <bgfx/bgfx.h>
#include <cstdint>
#include <iostream>
#include <string>

namespace pen::backend {
    // Global variables
    uint32_t framebuffer = 0; // In this case it is the view ID

    // FUNCTIONS
    // ---------

    // Shading and Uniforms
    // INFO: For some very cursed reason, it is impossible to make this function work
    uint16_t createUniform([[maybe_unused]] std::string name, [[maybe_unused]] UniformType type) {
        // bgfx::UniformType::Enum uType;

        // switch (type) {
        // case UniformType::Sampler:
        //     uType = bgfx::UniformType::Sampler;
        //     break;

        // case UniformType::Vec4:
        // case UniformType::Mat3:
        // case UniformType::Mat4:
        //     throw std::runtime_error("PENUMBRA_ERROR [BGFX]: Only Sampler Uniform is supported for now");
        //     break;
        // }

        // bgfx::UniformHandle uniform = bgfx::createUniform("s_color", bgfx::UniformType::Sampler);
        return bgfx::createUniform("s_color", bgfx::UniformType::Sampler).idx;
    }
    void deleteUniform([[maybe_unused]] uint16_t uniform) {
        bgfx::destroy((bgfx::UniformHandle)uniform);
    }
    // Ignored functions, BGFX doesn't need to create Bind Groups for each sprite like WebGPU
    uint16_t createSpriteUniform([[maybe_unused]] std::string name, [[maybe_unused]] UniformType type) {return 0;}
    void deleteSpriteUniform([[maybe_unused]] uint16_t uniform) {}

    void bindTexture(uint16_t texture) {
        std::cout << "PENUMBRA_CLUELESS: How did you manage to call this function? (bindTexture() in backendRender), trying to bind texture " << std::to_string(texture) << "\n";
    }

    // Framebuffer Stuff
    void bindFramebuffer(void* framebufferID) {
        framebuffer = *reinterpret_cast<uint32_t*>(framebufferID);
    }

    // General
    void setViewTransform(glm::mat4 viewMtx, glm::mat4 projMtx) {
        bgfx::setViewTransform(framebuffer, &viewMtx[0][0], &projMtx[0][0]);

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
    void setModelTransform(antumbra::Sprite* sprite) {
        bgfx::setTransform(sprite->transform.getMatrix());
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
        bgfx::setState((BGFX_STATE_DEFAULT // Use default
            ^ BGFX_STATE_WRITE_Z) // Remove Z
            | BGFX_STATE_BLEND_ALPHA // Enable Alpha
        );

        // Draw
        bgfx::submit(framebuffer, (bgfx::ProgramHandle)shader->getProgram());
    }
}