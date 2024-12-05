#include "utils/transform2D.hpp"
#include "utils/vectors.hpp"
#include <bx/math.h>
// TODO: Calculate matrix with GLM and not BX

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
        return matrix;
    }
    float* Transform2D::getMatrix() {
        calculateMatrix();
        return matrix;
    }


    void Transform2D::calculateMatrix() {
        // Boolean that indicates if we have to recalculate the main matrix
        bool needsRecalc = false;

        // If the position changed we calculate it
        if (position != _lastPos) {
            needsRecalc = true;
            _lastPos = position;
            bx::mtxTranslate(_posMtx, position.x, position.y, 0.0f);
        }
        // If the rotation changed we calculate it
        if (rotation != _lastRot) {
            needsRecalc = true;
            _lastRot = rotation;
            bx::mtxRotateZ(_rotMtx, rotation);
        }
        // If the scale changed we calculate it
        if (scale != _lastScl || deform != _lastDef) {
            needsRecalc = true;
            _lastScl = scale;
            _lastDef = deform;

            Vec2 totalScale = scale * deform;
            bx::mtxScale(_sclMtx, totalScale.x, totalScale.y, 1.0f);
        }

        // If the main matrix needs to be calculated, we calculate it
        if (needsRecalc) {
            bx::mtxIdentity(matrix);
            bx::mtxMul(matrix, _sclMtx, _rotMtx);
            bx::mtxMul(matrix, matrix, _posMtx);
        }
    }
    void Transform2D::forceCalculateMatrix() {
        _lastPos = position;
        bx::mtxTranslate(_posMtx, position.x, position.y, 0.0f);
        _lastRot = rotation;
        bx::mtxRotateZ(_rotMtx, rotation);
        _lastScl = scale;
        bx::mtxScale(_sclMtx, scale.x, scale.y, 1.0f);

        bx::mtxIdentity(matrix);
        bx::mtxMul(matrix, _sclMtx, _rotMtx);
        bx::mtxMul(matrix, matrix, _posMtx);
    }
}