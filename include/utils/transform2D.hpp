#pragma once

#include "utils/vectors.hpp"

namespace pen {
    class Transform2D {
    public:
        Transform2D();
        Transform2D(Vec2 pos, double rot = 0, Vec2 scl = Vec2(1));

        Vec2 position;
        double rotation;
        Vec2 scale;
    private:

    };
}