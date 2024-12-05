#pragma once

#include "backend/backendVtxLayout.hpp"
namespace pen::antumbra {
    struct PosUvVertex {
        float x;
        float y;
        float z;

        float u;
        float v;

        static backend::BackendVtxLayout getVertexLayout() {
            // Init vertex attributes
            backend::BackendVtxLayout vertexLayout;
            vertexLayout.addVtxAttrib(backend::Position);
            vertexLayout.addVtxAttrib(backend::TexCoord0);
            
            return vertexLayout;
        }
    };
}