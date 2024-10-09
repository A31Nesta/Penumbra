#pragma once

/**
    ANTUMBRA
    ---------------------------------------------------
    2D Renderer. It holds data necessary for rendering.

    The user can register, unregister and modify:
    - Sprites: A struct containing:
        - A Transform: Position, Rotation and Scale
        - A Deform: Scale applied to the original 1x1 sprite. It corrects Aspect Ratio to match the texture
        - A Texture ID
        - A Shader ID
    
    The renderer automatically manages:
    - Textures: A struct containing:
        - A path
        - A handle from the graphics library
        - The number of sprites using it. If no sprite is using it, it will get deleted
        - A Persistence flag. A boolean value indicating whether or not it will get deleted when it has no users
    - Shaders: A Struct containing:
        - A Path
        - A handle from the graphics library
        - The number of sprites using it.
        - A Persistence flag. Always true in the default shader
        - Functions to set uniforms
 */

#include "antumbra/types/shader.hpp"
#include "antumbra/types/texture.hpp"
#include "types/sprite.hpp"
#include "utils/transform2D.hpp"
#include "utils/vectors.hpp"

#include <bgfx/bgfx.h>
#include <cstdint>
#include <vector>
namespace pen::antumbra {
    class Antumbra {
    public:
        Antumbra(std::string defaultShader);

        Sprite& addSprite(std::string texture, Transform2D transform);
        Sprite& addSprite(std::string texture, Transform2D transform, std::string shader);
        Sprite& addSprite(std::string texture, Vec2 position, double rotation = 0, Vec2 scale = 0);
        Sprite& addSprite(std::string texture, Vec2 position, double rotation, Vec2 scale, std::string shader);

        void removeSprite(Sprite sprite);
        void removeSprite(uint32_t sprite);

        void draw(uint32_t view);
        
    private:
        // Inits the bgfx objects and loads the default shader
        void initQuad();

        std::vector<Sprite> sprites;
        std::vector<Texture> textures;
        std::vector<Shader> shaders;

        std::string defaultShaderPath;

        // Quad Vertex and Index Buffer Handles
        bgfx::VertexLayout vertexAttr;
        bgfx::IndexBufferHandle vbh;
        bgfx::IndexBufferHandle ibh;

        // Same for every object. Everything is based on squares in 2D
        static constexpr float QUAD_VTX[30] = {
            //   _______________    _________
            //  |    POSITION   |  |   UVs   |
                -.5f, 0.5f, 0.0f,  0.0f, 1.0f,	// top left
                0.5f, 0.5f, 0.0f,  1.0f, 1.0f,	// top right
                0.5f, -.5f, 0.0f,  1.0f, 0.0f,	// bottom right
                -.5f, -.5f, 0.0f,  0.0f, 0.0f	// bottom left 
            //  _________________  ___________
        };
        static constexpr uint16_t QUAD_IDX[6] = {
            0, 1, 2,
            2, 3, 4
        };
    };
}