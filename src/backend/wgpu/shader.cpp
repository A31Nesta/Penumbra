#include "../shader.hpp"
#include "backend/wgpu/wgpuutils/objectManager.hpp"

#include <cstdint>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <iostream>

// Test
#include <filesystem>

namespace pen {
    Shader::Shader(uint32_t id, std::string shader) {
        this->id = id;
        path = shader;

        std::cout << "PENUMBRA_INFO [WGPU]: Loading Shader: "+shader+"\n";

        // Load shader and create pipeline
        program = loadShader(shader+".wgsl");

        std::cout << "PENUMBRA_INFO [WGPU]: Render Pipeline Created! - "+std::to_string(program)+"\n";
    }
    Shader::~Shader() {
        backend::destroyRenderPipeline(id);
    }

    uint16_t Shader::loadShader(std::string name) {
        std::string shaderCode;
        std::ifstream file(name);
        if(file.is_open()) {
            std::stringstream buffer;
            buffer << file.rdbuf();
            shaderCode = buffer.str();
            file.close();
            // std::cout << "PENUMBRA_INFO [WGPU]: Shader read! Contents:\n" << shaderCode << "\n\n";
        }
        else {
            std::cout << "PENUMBRA_ERROR [WGPU]: UNABLE TO OPEN SHADER! Files in folder:\n";
            std::string curr_path = ".";
            for (const auto & entry : std::filesystem::directory_iterator(curr_path))
                std::cout << entry.path() << std::endl;
            throw std::runtime_error("PENUMBRA_ERROR: Couldn't open Shader "+name);
        }

        // Create Render Pipeline for this shader and get the "program"
        uint16_t pipeline = backend::createRenderPipeline(shaderCode);

        return pipeline;
    }
}