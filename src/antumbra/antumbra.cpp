#include "antumbra.hpp"

#include "antumbra/types/shader.hpp"
#include "antumbra/types/sprite.hpp"

#include "backend/texture.hpp"

#include "backend/backendRender.hpp"
#include "backend/backendVtxLayout.hpp"

#include "debug/log.hpp"
#include "utils/vectors.hpp"

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include <cstdint>
#include <string>
#include <vector>

namespace pen::antumbra {
    // Same for every object. Everything is based on squares in 2D
    const std::vector<float> QUAD_VTX {
        0.5f, 0.5f, 0.0f,  1.0f, 1.0f,	// top right
        0.5f, -.5f, 0.0f,  1.0f, 0.0f,	// bottom right
        -.5f, -.5f, 0.0f,  0.0f, 0.0f,	// bottom left
        -.5f, 0.5f, 0.0f,  0.0f, 1.0f 	// top left
    };
    const std::vector<uint16_t> QUAD_IDX {
        1, 0, 3,
        2, 1, 3
    };

    Antumbra::Antumbra(std::string defaultShader) {
        defaultShaderPath = defaultShader;

        Shader* defaultS = new Shader(0, defaultShaderPath);
        defaultS->setPersistence(true);
        shaders.push_back(defaultS);

        // Create Uniforms
        // TODO: Change magic string to defines
        colorUniform = bgfx::createUniform("s_color", bgfx::UniformType::Sampler);
        
        initQuad();
    }
    Antumbra::~Antumbra() {
        // Delete Sprites
        for (Sprite* sprite : sprites) {
            if (sprite != nullptr) {
                delete sprite;
            }
        }

        // Delete Shaders
        for (Shader* shader : shaders) {
            if (shader != nullptr) {
                delete shader;
            }
        }

        // Delete Textures
        for (Texture* texture : textures) {
            if (texture != nullptr) {
                delete texture;
            }
        }

        // Delete BGFX objects
        bgfx::destroy(colorUniform);
        delete bvb;
        delete bib;
    }

    // BASE ADD SPRITE
    Sprite* Antumbra::createSprite(std::string texture, Transform2D transform, std::string shader) {
        uint32_t id = sprites.size();

        Texture* t = getTexture(texture);
        Shader* s = getShader(shader);
        uint32_t textureID = t->isValid() ? t->getID() : 0;
        Vec2 deform = t->isValid() ? t->getDeform() : 1;

        uint32_t shaderID = s->getID();
        
        Sprite* sprite = new Sprite(id, transform, deform, textureID, shaderID);
        sprites.push_back(sprite);
        return sprite;
    }
    // OTHER ADD SPRITES
    Sprite* Antumbra::createSprite(std::string texture, Transform2D transform) {
        return createSprite(texture, transform, defaultShaderPath);
    }
    Sprite* Antumbra::createSprite(std::string texture, Vec2 position, double rotation, Vec2 scale) {
        return createSprite(texture, Transform2D(position, rotation, scale), defaultShaderPath);
    }
    Sprite* Antumbra::createSprite(std::string texture, Vec2 position, double rotation, Vec2 scale, std::string shader) {
        return createSprite(texture, Transform2D(position, rotation, scale), shader);
    }
    // REMOVE SPRITES
    // TODO: IMLEMENT REMOVE SPRITE!
    void Antumbra::removeSprite(Sprite sprite) {}
    void Antumbra::removeSprite(uint32_t sprite) {}

    // The Final Boss.
    // Draw a fucking scene
    void Antumbra::draw(uint32_t framebuffer, uint16_t width, uint16_t height) {
        const glm::vec3 at(0.0f, 0.0f, 0.0f);
        const glm::vec3 eye(0.0f, 0.0f, 2.0f);

        // Set view and projection matrix for view 0.
        glm::mat4 viewMtx = glm::lookAt(eye, at, glm::vec3(0, 1, 0));
        // TODO: Make ortho camera work. At least with BGFX Backend it doesn't work
        // glm::mat4 projMtx = glm::ortho(
        //     -float(width)/float(height), float(width)/float(height),
        //     -float(height)/float(height), float(height)/float(height),
        //     0.1f, 100.0f
        // );
        glm::mat4 projMtx = glm::perspective(glm::radians(60.0f), float(width)/float(height), 0.1f, 100.f);

        backend::setViewTransform(framebuffer, &viewMtx[0][0], &projMtx[0][0]);

        for (Sprite* sprite : sprites) {
            backend::setModelTransform(sprite->transform);

            // Buffers
            backend::setBuffers(bvb, bib);

            // Set Texture
            textures.at(sprite->getTextureID())->bindTexture();

            // Set render state and draw
      	    bgfx::setState(BGFX_STATE_DEFAULT // Use default
                ^ BGFX_STATE_WRITE_Z // Remove Z
                | BGFX_STATE_BLEND_ALPHA // Enable Alpha
            );
            bgfx::submit(framebuffer, shaders.at(sprite->getShaderID())->getProgram());
        }

        // bgfx::submit(view, shaders.at(0).getProgram());
    }


    // Private

    // Inits the bgfx objects and loads the default shader
    void Antumbra::initQuad() {
        backend::BackendVtxLayout vtxLayout;
        vtxLayout.addVtxAttrib(backend::Position);
        vtxLayout.addVtxAttrib(backend::TexCoord0);

        bvb = new backend::BackendVtxBuffer(QUAD_VTX, vtxLayout);
        bib = new backend::BackendIdxBuffer(QUAD_IDX);

        debug::print("\n\nSIZEOF QUAD_VTX: "+std::to_string(QUAD_VTX.size() * sizeof(float))+"\n");
    }

    Shader* Antumbra::getShader(std::string shader) {
        for (Shader* s : shaders) {
            if (s->getPath() == shader) {
                s->incrementUsers();
                return s;
            }
        }

        Shader* newShader = new Shader(shaders.size(), shader);
        shaders.push_back(newShader);
        return newShader;
    }
    Texture* Antumbra::getTexture(std::string texture) {
        for (Texture* t : textures) {
            if (t->getPath() == texture) {
                t->incrementUsers();
                return t;
            }
        }

        Texture* newTexture = new Texture(textures.size(), texture, PENUMBRA_TEX_COLOR, colorUniform.idx);
        
        // Only add if it's Valid
        if (newTexture->isValid()) {
            textures.push_back(newTexture);
        }

        return newTexture;
    }
}