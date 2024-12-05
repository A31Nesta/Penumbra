#pragma once

#include "utils/vectors.hpp"

#include <cstdint>
#include <string>

#define PENUMBRA_TEX_COLOR 0
#define PENUMBRA_TEX_NORMAL 1
#define PENUMBRA_TEX_ROUGH 2
#define PENUMBRA_TEX_METAL 3

namespace pen {
    class Texture {
    public:
        Texture(uint32_t id, std::string path, uint8_t type, uint16_t uniform);
        ~Texture();

        void incrementUsers() { users++; }
        void decrementUsers() { users--; }

        void bindTexture();

        bool isValid() { return valid; }
        uint32_t getID() { return id; }
        std::string getPath() { return path; }

        Vec2 getDeform() { return spriteDeform; }

    private:
        void loadTexture(std::string path);

        bool valid = true;

        std::string path;
        uint32_t users = 1; // Initialized to 1 because at the time of creation 1 object would be using it
        bool persistent = false;

        // Texture Data
        uint32_t _textureHandle;
        uint16_t width, height;
        Vec2 spriteDeform;
        uint8_t textureType;

        // Uniform
        uint16_t uniform;

        uint32_t id;
    };
}