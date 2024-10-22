#include "sprite.hpp"
#include <bgfx/bgfx.h>
#include <bx/math.h>

namespace pen::antumbra {
    Sprite::Sprite(uint32_t id, Transform2D tr, Vec2 df, uint32_t tid, uint32_t sid) {
        this->id = id;
        transform = tr;
        transform.setDeform(df);
        textureID = tid;
        shaderID = sid;
    }

    void Sprite::setDeform(Vec2 deform) {
        transform.setDeform(deform);
    }
}