#pragma once

#include <glm/glm.hpp>

/**
    So very surprisingly, this is just a wrapper for GLM stuff
    It exposes X and Y coordinates and functions but it's actually
    just GLM.
 */

namespace pen {
    // Vector2
    class Vec2 {
    public:
        Vec2();
        Vec2(double xy);
        Vec2(double x, double y);
        Vec2(glm::dvec2 vec);

        // Convert from GLM vec2
        void operator=(glm::dvec2 v);
        // Convert to GLM vec2
        operator glm::dvec2();

        // Compare Vectors
        bool operator==(Vec2 v2);
        Vec2 operator*(Vec2 v2);
        Vec2 operator*(double d);
        Vec2 operator+(Vec2 v2);
        Vec2 operator+(double d);
        Vec2 operator-(Vec2 v2);
        Vec2 operator-(double d);

        // Coords
        double x;
        double y;
    private:
        void updateVector();

        glm::dvec2 vector;
    };

    // Vector3
    class Vec3 {
    public:
        Vec3();
        Vec3(double xyz);
        Vec3(double x, double y, double z);
        Vec3(glm::dvec3 vec);

        // Convert from GLM vec3
        void operator=(glm::dvec3 v);
        // Convert to GLM vec3
        operator glm::dvec3();

        // Compare Vectors
        bool operator==(Vec3 v3);
        Vec3 operator*(Vec3 v3);
        Vec3 operator*(double d);
        Vec3 operator+(Vec3 v3);
        Vec3 operator+(double d);
        Vec3 operator-(Vec3 v3);
        Vec3 operator-(double d);

        // Coords
        double x;
        double y;
        double z;
    private:
        void updateVector();

        glm::dvec3 vector;
    };
}