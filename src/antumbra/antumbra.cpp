#include "antumbra.hpp"
#include <bgfx/bgfx.h>

namespace pen::antumbra {
    Antumbra::Antumbra(std::string defaultShader) {
        defaultShaderPath = defaultShader;
        initQuad();
    }

    // BASE ADD SPRITE
    Sprite& Antumbra::addSprite(std::string texture, Transform2D transform, std::string shader) {
        // TODO: Fucking delete this shit and do something normal for once
        return *new Sprite();
    }
    // OTHER ADD SPRITES
    Sprite& Antumbra::addSprite(std::string texture, Transform2D transform) {
        return addSprite(texture, transform, defaultShaderPath);
    }
    Sprite& Antumbra::addSprite(std::string texture, Vec2 position, double rotation, Vec2 scale) {
        return addSprite(texture, Transform2D(position, rotation, scale), defaultShaderPath);
    }
    Sprite& Antumbra::addSprite(std::string texture, Vec2 position, double rotation, Vec2 scale, std::string shader) {
        return addSprite(texture, Transform2D(position, rotation, scale), shader);
    }


    // Private

    // Inits the bgfx objects and loads the default shader
    void Antumbra::initQuad() {
        // Init vertex attributes
        vertexAttr
            .begin()
            .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .end();
    }
}