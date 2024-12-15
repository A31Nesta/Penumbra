#include "antumbra.hpp"

#include "antumbra/types/sprite.hpp"

#include "backend/backendRender.hpp"
#include "backend/backendVtxLayout.hpp"

#include "debug/log.hpp"
#include "utils/vectors.hpp"

#include <glm/fwd.hpp>
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

#ifdef PENUMBRA_BACKEND_BGFX
    #include <bgfx/bgfx.h>
#endif

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

        // Create 2D Vertex Layout
        // When you do this step doesn't matter in BGFX
        // In WGPU, this has to be done before creating the Pipeline (Shader)
        backend::BackendVtxLayout vtxLayout;
        vtxLayout.addVtxAttrib(backend::Position);
        vtxLayout.addVtxAttrib(backend::TexCoord0);
        // This method creates the backend-specific object. Once this function
        // gets called it is impossible to modify the Vertex Layout
        // [WGPU]: With the WGPU backend enabled this also sets the current
        // Vertex Layout. Pipelines created after this is called will use
        // this Vertex Layout
        vtxLayout.getBackendSpecificData();

        // Create Quad
        Shader* defaultS = new Shader(0, defaultShaderPath);
        defaultS->setPersistence(true);
        shaders.push_back(defaultS);

        // Create Uniforms
        // HACK: With BGFX Backend we manually create the Uniform here

        // EXPLANATION: backend::createUniform() crashes or for whatever reason creates
        // a uniform that is invalid and curses the entire code. Creating a uniform with
        // backend::createUniform() makes things like deleting BackendVertexBuffer crash
        // with a "free(): invalid pointer". Something sussy is going on with BGFX.
        // This strange band-aid solution appears to "solve" it.
        // Literally a coconut.vtf moment.
        // EDIT: Editing the backend::createUniform() function to take 0 parameters
        // makes it work. What the fuck?

        // TODO: Investigate issue with BGFX Shader Uniform Creation
        #ifdef PENUMBRA_BACKEND_BGFX
            // TODO: Change magic string to defines
            // colorUniform = backend::createUniform("s_color", backend::UniformType::Sampler);
            colorUniform = bgfx::createUniform("s_color", bgfx::UniformType::Sampler).idx;
        #else
            colorUniform = backend::createUniform("s_color", backend::UniformType::Sampler);
        #endif
        
        // We pass the Vertex Layout to the initQuad() function
        initQuad(vtxLayout);
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

        // Delete Vertex and Index buffers + Uniform
        backend::deleteUniform(colorUniform);
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

    // Draw the scene
    void Antumbra::draw(uint16_t width, uint16_t height) {
        glm::vec3 at(0.0f, 0.0f, 0.0f);
        glm::vec3 eye(0.0f, 0.0f, 2.0f);

        glm::vec3 up(0, 1, 0);

        // Set view and projection matrix for view 0.
        glm::mat4 viewMtx = glm::lookAt(eye, at, up);
        // TODO: Make ortho camera work. At least with BGFX Backend it doesn't work
        // glm::mat4 projMtx = glm::ortho(
        //     -float(width)/float(height), float(width)/float(height),
        //     -float(height)/float(height), float(height)/float(height),
        //     0.1f, 100.0f
        // );
        glm::mat4 projMtx = glm::perspective(glm::radians(60.0f), float(width)/float(height), 0.1f, 100.f);

        // Set View and Projection
        backend::setViewTransform(viewMtx, projMtx);

        for (Sprite* sprite : sprites) {
            backend::setModelTransform(sprite);

            // Buffers
            backend::setBuffers(bvb, bib);

            // Set Texture
            if (textures.size() > sprite->getTextureID()) {
                textures.at(sprite->getTextureID())->bindTexture();
            }

            // Draw
            backend::drawCurrent(shaders.at(sprite->getShaderID()));
        }
    }


    // Private

    // Inits the Vertex and Index Buffers and loads the default shader
    void Antumbra::initQuad(backend::BackendVtxLayout vtxLayout) {
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

        Texture* newTexture = new Texture(textures.size(), texture, PENUMBRA_TEX_COLOR, colorUniform);
        
        // Only add if it's Valid
        if (newTexture->isValid()) {
            textures.push_back(newTexture);
        }

        return newTexture;
    }
}