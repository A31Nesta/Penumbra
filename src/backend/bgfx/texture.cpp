#include "../texture.hpp"
#include "debug/consoleColors.hpp"
#include "debug/log.hpp"
#include "utils/vectors.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include <bgfx/bgfx.h>

// STBI
#define STB_IMAGE_IMPLEMENTATION
#include "stbi/stb_image.h"

namespace pen {
    Texture::Texture(uint32_t id, std::string path, uint8_t type, uint16_t uniform) {
        this->id = id;
        loadTexture(path);

        textureType = type;
        this->uniform = uniform;
        
        this->path = path;
    }
    Texture::~Texture() {
        // DO NOT DESTROY UNIFORM
        // That's Antumbra's job, not ours, we just delete our texture
        bgfx::destroy((bgfx::TextureHandle)_textureHandle);
    }

    void Texture::bindTexture() {
        // Bind texture
        bgfx::setTexture(textureType, (bgfx::UniformHandle)uniform, (bgfx::TextureHandle)_textureHandle);
    }

    void Texture::loadTexture(std::string path) {
        // Khronos Texture
        // Native BGFX format according to this realiable source:
        // https://github.com/beardsvibe/leengine/blob/master/src/render.c#L115
        size_t dotPos = path.find_last_of('.');
        if (dotPos != std::string::npos && path.substr(dotPos) == ".ktx") {
            std::ifstream file(path);
            size_t fileSize;

            // TODO: Test cast bgfx memory uint8_t* into char*
            // What the fuck does this mean???
            if (file.is_open()) {
                file.seekg(0, std::ios::end);
                fileSize = file.tellg();
                char* data = (char*)malloc(fileSize); // Create data pointer

                file.seekg(0, std::ios::beg);
                file.read(data, fileSize);
                file.close();

                const bgfx::Memory* mem = bgfx::copy(data, fileSize);
                free(data); // free data pointer

                bgfx::TextureInfo texInfo;
                _textureHandle = bgfx::createTexture(mem, BGFX_TEXTURE_NONE|BGFX_SAMPLER_NONE, 0, &texInfo).idx;
                width = texInfo.width;
                height = texInfo.height;

                std::cout << "Loaded texture with " << std::to_string(texInfo.numMips) << " mips.\tTexture Type: " << std::to_string(texInfo.format) << "\n";
            }
        }
        else {
            int x = 0;
            int y = 0;
            int channels = 0;
            stbi_uc* bytes = stbi_load(path.c_str(), &x, &y, &channels, 4);

            if (bytes) {
                // Totally not yoinked from here:
                // https://github.com/beardsvibe/leengine/blob/master/src/render.c#L115
                // We do x*y*4 here because per pixel we have RGB and A
                _textureHandle = bgfx::createTexture2D(x, y, false, 1, bgfx::TextureFormat::RGBA8, BGFX_TEXTURE_NONE|BGFX_SAMPLER_NONE, bgfx::copy(bytes, x * y * 4)).idx;
                width = x;
                height = y;
                stbi_image_free(bytes);
            }
        }

        // Le fnuui error handling
        if (!bgfx::isValid((bgfx::TextureHandle)_textureHandle) || width == 0 || height == 0) {
            debug::print("Failed to load Texture: " + path + "\n", debug::Color::RED);
            valid = false;
        } else {
            // calculate deform
            uint16_t maxValue = std::max(width, height);
            spriteDeform.x = float(width)/maxValue;
            spriteDeform.y = float(height)/maxValue;
        }

    }
}