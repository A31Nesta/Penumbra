#include "../texture.hpp"
#include "backend/backendRender.hpp"
#include "backend/wgpu/wgpuutils/objectManager.hpp"
#include "debug/consoleColors.hpp"
#include "debug/log.hpp"

#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

#include <iostream>

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
        // Delete Texture
        backend::destroyTexture(_textureHandle);
    }

    void Texture::bindTexture() {
        // Bind texture
        backend::bindTexture(_textureHandle);
    }

    void Texture::loadTexture(std::string path) {
        // Normal ahh PNGs and JPGs
        int x = 0;
        int y = 0;
        int channels = 0;
        stbi_set_flip_vertically_on_load(true);
        uint8_t* bytes = stbi_load(path.c_str(), &x, &y, &channels, 4);

        // Load texture into WGPU
        if (bytes) {
            std::cout << "PENUMBRA_INFO [WGPU]: Loaded texture into memory!\n";
            // We do x*y*4 here because per pixel we have RGB and A
            std::vector<uint8_t> data(bytes, bytes+x*y*4);
            _textureHandle = backend::createTexture(data, x, y);
            width = x;
            height = y;
            stbi_image_free(bytes);

            // calculate deform
            uint16_t maxValue = std::max(width, height);
            spriteDeform.x = float(width)/maxValue;
            spriteDeform.y = float(height)/maxValue;
        }
        else {
            std::cout << "PENUMBRA_ERROR [WGPU]: Failed to load Texture: " + path + "\n";
            valid = false;
        }
    }
}