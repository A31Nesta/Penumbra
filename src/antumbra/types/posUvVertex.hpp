#pragma once

#include <bgfx/bgfx.h>
namespace pen::antumbra {
    struct PosUvVertex {
        float x;
        float y;
        float z;

        float u;
        float v;

        static bgfx::VertexLayout getVertexLayout() {
            // Init vertex attributes
            bgfx::VertexLayout vertexLayout;
            vertexLayout.begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();
            return vertexLayout;
        }
    };
}