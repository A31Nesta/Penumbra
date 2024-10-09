#include "sprite.hpp"
#include <bgfx/bgfx.h>
#include <bx/math.h>

namespace pen::antumbra {
    Sprite::Sprite(uint32_t id, Transform2D tr, Vec2 df, uint32_t tid, uint32_t sid) {
        this->id = id;
        transform = tr;
        deform = df;
        textureID = tid;
        shaderID = sid;
        prepareMatrix();
    }

    void Sprite::loadMatrix() {
        bgfx::setTransform(mtx);
    }

    void Sprite::prepareMatrix() {
        bx::mtxRotateY(mtx, transform.rotation);

        // position x,y,z
        mtx[12] = transform.position.x;
        mtx[13] = transform.position.y;
        mtx[14] = 0.0f;
    }
}