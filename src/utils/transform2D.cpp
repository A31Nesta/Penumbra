#include "utils/transform2D.hpp"
#include "utils/vectors.hpp"

#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>

namespace pen {
    Transform2D::Transform2D() : position(), scale() {
        rotation = 0;
        forceCalculateMatrix();
    }
    Transform2D::Transform2D(Vec2 pos, double rot, Vec2 scl) : position(pos), scale(scl) {
        rotation = rot;
        forceCalculateMatrix();
    }

    // set Deform
    void Transform2D::setDeform(Vec2 def) {
        deform = def;
        calculateMatrix();
    }

    Transform2D::operator float*() {
        calculateMatrix();
        return &matrix[0][0];
    }
    float* Transform2D::getMatrix() {
        calculateMatrix();
        return &matrix[0][0];
    }


    void Transform2D::calculateMatrix() {
        // Boolean that indicates if we have to recalculate the main matrix
        bool needsRecalc = false;

        // If the position changed we calculate it
        if (position != _lastPos) {
            needsRecalc = true;
            _lastPos = position;
            _posMtx = glm::translate(glm::mat4(1), glm::vec3(position.x, position.y, 0.0f));
        }
        // If the rotation changed we calculate it
        if (rotation != _lastRot) {
            needsRecalc = true;
            _lastRot = rotation;
            _rotMtx = glm::rotate(glm::mat4(1), float(rotation), glm::vec3(0, 0, 1));
        }
        // If the scale changed we calculate it
        if (scale != _lastScl || deform != _lastDef) {
            needsRecalc = true;
            _lastScl = scale;
            _lastDef = deform;

            Vec2 totalScale = scale * deform;
            _sclMtx = glm::scale(glm::mat4(1), glm::vec3(totalScale.x, totalScale.y, 1.0f));
        }

        // If the main matrix needs to be calculated, we calculate it
        if (needsRecalc) {
            matrix = _sclMtx * _rotMtx * _posMtx;
        }
    }
    void Transform2D::forceCalculateMatrix() {
        _lastPos = position;
        _posMtx = glm::translate(glm::mat4(1), glm::vec3(position.x, position.y, 0.0f));
        _lastRot = rotation;
        _rotMtx = glm::rotate(glm::mat4(1), float(rotation), glm::vec3(0, 0, 1));
        _lastScl = scale;
        _sclMtx = glm::scale(glm::mat4(1), glm::vec3(scale.x, scale.y, 1.0f));

        matrix = _sclMtx * _rotMtx * _posMtx;
    }
}