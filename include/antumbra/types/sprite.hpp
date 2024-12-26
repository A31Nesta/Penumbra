#pragma once

#include "../../utils/transform2D.hpp"
#include "../../utils/vectors.hpp"
#include <cstdint>

namespace pen::antumbra {
    class Sprite {
    public:
        Sprite(uint32_t id, Transform2D tr, Vec2 df, uint32_t tid, uint32_t sid);
        ~Sprite(); // to delete backend-specific data

        void setDeform(Vec2 deform);
        
        uint32_t getID() { return id; }
        uint32_t getShaderID() { return shaderID; }
        uint32_t getTextureID() { return textureID; }

        uint16_t _getUniformData() { return uniform; }

        Transform2D transform;
    private:
        uint32_t id;
        uint32_t textureID;
        uint32_t shaderID;

        // Uniform
        // In BGFX this is nothing, in WGPU this is the Bind Group
        uint16_t uniform;
    };
}