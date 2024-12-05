#pragma once

#include "vectors.hpp"

#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_float4x4.hpp>

namespace pen {
    class Transform2D {
    public:
        Transform2D();
        Transform2D(Vec2 pos, double rot = 0, Vec2 scl = Vec2(1));

        // set Deform
        void setDeform(Vec2 def = Vec2(1));

        // Get Matrix
        operator float*();
        float* getMatrix();

        Vec2 position;
        double rotation;
        Vec2 scale;
    private:
        void calculateMatrix();
        void forceCalculateMatrix();

        // Deform Value
        // Gets multiplied by the scale in order to correct
        // aspect ratios
        Vec2 deform = Vec2(1);

        // Matrices
        // MAIN
        glm::mat4 matrix;
        // POS/ROT/SCALE
        glm::mat4 _posMtx;
        glm::mat4 _rotMtx;
        glm::mat4 _sclMtx;

        // Last values used for matrix transform
        Vec2 _lastPos;
        double _lastRot;
        Vec2 _lastScl;
        Vec2 _lastDef; // Active deform Vector
    };
}