#include "antumbra.hpp"
#include "antumbra/types/posUvVertex.hpp"
#include "antumbra/types/shader.hpp"
#include "antumbra/types/sprite.hpp"
#include "debug/log.hpp"
#include "utils/vectors.hpp"

#include <bgfx/bgfx.h>
#include <bx/math.h>

#include <cstdint>
#include <string>

namespace pen::antumbra {
    // Same for every object. Everything is based on squares in 2D
    const PosUvVertex QUAD_VTX[] = {
        {0.5f, 0.5f, 0.0f,  1.0f, 1.0f},	// top right
        {0.5f, -.5f, 0.0f,  1.0f, 0.0f},	// bottom right
        {-.5f, -.5f, 0.0f,  0.0f, 0.0f},	// bottom left
        {-.5f, 0.5f, 0.0f,  0.0f, 1.0f} 	// top left
    };
    const uint16_t QUAD_IDX[] = {
        1, 0, 3,
        2, 1, 3
    };

    Antumbra::Antumbra(std::string defaultShader) {
        defaultShaderPath = defaultShader;

        Shader defaultS(0, defaultShaderPath);
        defaultS.setPersistence(true);
        shaders.push_back(defaultS);

        initQuad();
    }

    // BASE ADD SPRITE
    Sprite* Antumbra::addSprite(std::string texture, Transform2D transform, std::string shader) {
        uint32_t id = sprites.size();
        Vec2 deform = 0;
        uint32_t textureID = 0;
        Shader s = getShader(shader);
        
        Sprite* sprite = new Sprite(id, transform, deform, textureID, s.getID());
        sprites.push_back(sprite);
        return sprite;
    }
    // OTHER ADD SPRITES
    Sprite* Antumbra::addSprite(std::string texture, Transform2D transform) {
        return addSprite(texture, transform, defaultShaderPath);
    }
    Sprite* Antumbra::addSprite(std::string texture, Vec2 position, double rotation, Vec2 scale) {
        return addSprite(texture, Transform2D(position, rotation, scale), defaultShaderPath);
    }
    Sprite* Antumbra::addSprite(std::string texture, Vec2 position, double rotation, Vec2 scale, std::string shader) {
        return addSprite(texture, Transform2D(position, rotation, scale), shader);
    }
    // REMOVE SPRITES
    // TODO: IMLEMENT REMOVE SPRITE!
    void Antumbra::removeSprite(Sprite sprite) {}
    void Antumbra::removeSprite(uint32_t sprite) {}

    // The Final Boss.
    // Draw a fucking scene
    void Antumbra::draw(uint32_t view, uint16_t width, uint16_t height) {
        const bx::Vec3 at  = { 0.0f, 0.0f, 0.0f };
        const bx::Vec3 eye = { 0.0f, 0.0f, -2.0f };

        // Set view and projection matrix for view 0.
        float viewMtx[16];
        bx::mtxLookAt(viewMtx, eye, at);

        float projMtx[16];
        bx::mtxProj(projMtx,
                    60.0f,
                    float(width)/float(height),
                    0.1f, 100.0f,
                    bgfx::getCaps()->homogeneousDepth);

        bgfx::setViewTransform(view, viewMtx, projMtx);

        for (Sprite* sprite : sprites) {
            bgfx::setTransform(sprite->transform);

            // Buffers
            bgfx::setVertexBuffer(0, vbh);
            bgfx::setIndexBuffer(ibh);
            // Set render state and draw
      	    bgfx::setState(BGFX_STATE_DEFAULT);
            bgfx::submit(view, shaders.at(sprite->getShaderID()).getProgram());
        }

        // bgfx::submit(view, shaders.at(0).getProgram());
    }


    // Private

    // Inits the bgfx objects and loads the default shader
    void Antumbra::initQuad() {
        const bgfx::VertexLayout vtxLayout = PosUvVertex::getVertexLayout();
        
        vbh = bgfx::createVertexBuffer(bgfx::makeRef(QUAD_VTX, sizeof(QUAD_VTX)), vtxLayout);
        ibh = bgfx::createIndexBuffer(bgfx::makeRef(QUAD_IDX, sizeof(QUAD_IDX)));

        debug::print("\n\nSIZEOF QUAD_VTX: "+std::to_string(sizeof(QUAD_VTX))+"\n");
    }

    Shader Antumbra::getShader(std::string shader) {
        for (Shader s : shaders) {
            if (s.getPath() == shader) {
                s.incrementUsers();
                return s;
            }
        }

        Shader newShader(shaders.size(), shader);
        shaders.push_back(newShader);
        return newShader;
    }
}