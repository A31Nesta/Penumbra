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

#include "antumbra/types/sprite.hpp"

#include "backend/backendVtxLayout.hpp"
#include "backend/shader.hpp"
#include "backend/texture.hpp"
#include "backend/backendIdxBuffer.hpp"
#include "backend/backendVtxBuffer.hpp"

#include "utils/transform2D.hpp"
#include "utils/vectors.hpp"

#include <cstdint>
#include <vector>

namespace pen::antumbra {
    class Antumbra {
    public:
        Antumbra(std::string defaultShader);
        ~Antumbra();

        Sprite* createSprite(std::string texture, Transform2D transform);
        Sprite* createSprite(std::string texture, Transform2D transform, std::string shader);
        Sprite* createSprite(std::string texture, Vec2 position, double rotation = 0, Vec2 scale = Vec2(1));
        Sprite* createSprite(std::string texture, Vec2 position, double rotation, Vec2 scale, std::string shader);

        void removeSprite(Sprite sprite);
        void removeSprite(uint32_t sprite);

        void draw(uint16_t width, uint16_t height);
        
    private:
        // Inits the bgfx objects and loads the default shader
        void initQuad(backend::BackendVtxLayout vtxLayout);

        // Utitities
        Shader* getShader(std::string shader);
        Texture* getTexture(std::string texture);

        std::vector<Sprite*> sprites;
        std::vector<Texture*> textures;
        std::vector<Shader*> shaders;

        std::string defaultShaderPath;

        // Quad Vertex and Index Buffer Handles
        // ------------------------------------

        // Backend Vertex Buffer
        backend::BackendVtxBuffer* bvb;
        // Backend Index Buffer
        backend::BackendIdxBuffer* bib;

        // Uniform
        uint16_t colorUniform;
    };
}