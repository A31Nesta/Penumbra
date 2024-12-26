#include "antumbra/types/sprite.hpp"
#include "backend/backendRender.hpp"
#include <glm/fwd.hpp>

namespace pen::antumbra {
    Sprite::Sprite(uint32_t id, Transform2D tr, Vec2 df, uint32_t tid, uint32_t sid) {
        this->id = id;
        transform = tr;
        transform.setDeform(df);
        textureID = tid;
        shaderID = sid;

        // Creates Bind Group in WGPU, does nothing in BGFX.
        // In BGFX, the Uniform is created once; it's like creating the Layout
        uniform = backend::createSpriteUniform("modelMatrix", backend::UniformType::Mat4);
    }

    Sprite::~Sprite() {
        backend::deleteSpriteUniform(uniform);
    }

    void Sprite::setDeform(Vec2 deform) {
        transform.setDeform(deform);
    }
}