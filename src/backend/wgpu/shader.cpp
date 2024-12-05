#include "../shader.hpp"
#include "debug/log.hpp"

#include <fstream>
#include <stdexcept>
#include <string>

namespace pen {
    Shader::Shader(uint32_t id, std::string shader) {
        this->id = id;
        path = shader;

        debug::print("\n\n--- LOADING SHADER: "+shader+" ---\n");

        // Load shaders and create program here

        debug::print("SHADER PROGRAM CREATED! - "+std::to_string(program)+"\n");
        // Check if shader is valid here (if possible)
    }
    Shader::~Shader() {
        // Delete program
    }

    uint16_t Shader::loadShader(std::string name) {
        char* data = new char[2048];
        std::ifstream file;
        size_t fileSize;
        file.open(name);
        if(file.is_open()) {
            file.seekg(0, std::ios::end);
            fileSize = file.tellg();
            file.seekg(0, std::ios::beg);
            file.read(data, fileSize);
            file.close();
        }
        else {
            throw std::runtime_error("PENUMBRA_ERROR: Couldn't open Shader "+name);
        }

        // Load (however tf is done)

        // Check if it's valid (if possible)

        return 0;
    }
}