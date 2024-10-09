#include "utils/transform2D.hpp"

namespace pen {
    Transform2D::Transform2D() : position(), scale() {
        rotation = 0;
    }
    Transform2D::Transform2D(Vec2 pos, double rot, Vec2 scl) : position(pos), scale(scl) {
        rotation = rot;
    }
}