#pragma once

#include "utils/transform2D.hpp"
#include <cstdint>

namespace pen::antumbra {
    class Sprite {
    public:
        Sprite(uint32_t id, Transform2D tr, Vec2 df, uint32_t tid, uint32_t sid);
        
        uint32_t getShaderID() { return shaderID; }
        uint32_t getTextureID() { return textureID; }

        Transform2D transform;
        Vec2 deform;
    private:
        uint32_t id;
        uint32_t textureID;
        uint32_t shaderID;
    };
}