#pragma once

#include <bgfx/bgfx.h>
#include <cstdint>
#include <string>

namespace pen {
    class Texture {
    public:
        Texture(uint32_t id, std::string path);

        void incrementUsers() { users++; }
        void decrementUsers() { users--; }

        bool isValid() { return valid; }
        uint32_t getID() { return id; }
        std::string getPath() { return path; }

    private:
        void loadTexture(std::string path);

        bool valid = true;

        std::string path;
        uint32_t users = 1; // Initialized to 1 because at the time of creation 1 object would be using it
        bool persistent = false;

        // Texture Data
        bgfx::TextureHandle _bgfxTex;
        uint16_t width, height;

        uint32_t id;
    };
}