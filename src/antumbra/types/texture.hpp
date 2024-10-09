#pragma once

#include <cstdint>
#include <string>

namespace pen::antumbra {
    class Texture {
    public:

    private:
        std::string path;
        uint32_t users = 1; // Initialized to 1 because at the time of creation 1 object would be using it
        bool persistent = false;

        uint32_t id;
    };
}