#pragma once

#include "utils/transform2D.hpp"
#include <cstdint>

namespace pen::antumbra {
    class Sprite {
    public:
        Transform2D transform;
        Vec2 deform;
    private:
        uint32_t id;
        uint32_t textureID;
        uint32_t shaderID;
    };
}