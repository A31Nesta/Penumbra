#include "../texture.hpp"
#include "debug/consoleColors.hpp"
#include "debug/log.hpp"

#include <cstdint>
#include <cstdlib>
#include <string>

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
        
        // Delete Texture
    }

    void Texture::bindTexture() {
        // Bind texture
    }

    void Texture::loadTexture(std::string path) {
        // Normal ahh PNGs and JPGs
        int x = 0;
        int y = 0;
        int channels = 0;
        stbi_uc* bytes = stbi_load(path.c_str(), &x, &y, &channels, 4);

        // Load texture into WGPU somehow

        // Le fnuui error handling
        // Check if the image is valid. Replace the "true" with the check lol
        if (true) {
            debug::print("Failed to load Texture: " + path + "\n", debug::Color::RED);
            valid = false;
        } else {
            // calculate deform
            // uint16_t maxValue = std::max(width, height);
            // spriteDeform.x = float(width)/maxValue;
            // spriteDeform.y = float(height)/maxValue;
        }

    }
}