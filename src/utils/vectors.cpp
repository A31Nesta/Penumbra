#include "utils/vectors.hpp"

namespace pen {
    // VECTOR 2
    // Constructors
    Vec2::Vec2() {
        x = 0;
        y = 0;
        updateVector();
    }
    Vec2::Vec2(double xy) {
        x = xy;
        y = xy;
        updateVector();
    }
    Vec2::Vec2(double x, double y) {
        this->x = x;
        this->y = y;
        updateVector();
    }
    Vec2::Vec2(glm::dvec2 vec) {
        x = vec.x;
        y = vec.y;
        vector = vec;
    }

    // Convert from GLM vec2
    void Vec2::operator=(glm::dvec2 v) {
        x = v.x;
        y = v.y;
        vector = v;
    }
    // Convert to GLM vec2
    Vec2::operator glm::dvec2() {
        return vector;
    }

    // Compare Vectors
    bool Vec2::operator==(Vec2 v2) {
        return x == v2.x && y == v2.y;
    }
    Vec2 Vec2::operator*(Vec2 v2) {
        return Vec2(x * v2.x, y * v2.y);
    }


    // Functions
    void Vec2::updateVector() {
        vector.x = x;
        vector.y = y;
    }

    // END VECTOR 2

    // VECTOR 3
    // Constructors
    Vec3::Vec3() {
        x = 0;
        y = 0;
        z = 0;
        updateVector();
    }
    Vec3::Vec3(double xyz) {
        this->x = xyz;
        this->y = xyz;
        this->z = xyz;
        updateVector();
    }
    Vec3::Vec3(double x, double y, double z) {
        this->x = x;
        this->y = y;
        this->z = z;
        updateVector();
    }
    Vec3::Vec3(glm::dvec3 vec) {
        this->x = vec.x;
        this->y = vec.y;
        this->z = vec.z;
        vector = vec;
    }

    // Convert from GLM vec3
    void Vec3::operator=(glm::dvec3 v) {
        x = v.x;
        y = v.y;
        z = v.z;
        vector = v;
    }
    // Convert to GLM vec3
    Vec3::operator glm::dvec3() {
        return vector;
    }

    // Compare Vectors
    bool Vec3::operator==(Vec3 v3) {
        return x == v3.x && y == v3.y && z == v3.z;
    }
    Vec3 Vec3::operator*(Vec3 v3) {
        return Vec3(x * v3.x, y * v3.y, z * v3.z);
    }

    // Functions
    void Vec3::updateVector() {
        vector.x = x;
        vector.y = y;
        vector.z = z;
    }
    // END VECTOR 3
}