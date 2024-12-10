#include "../shader.hpp"
#include "backend/wgpu/wgpuutils/objectManager.hpp"
#include "debug/log.hpp"

#include <cstdint>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace pen {
    Shader::Shader(uint32_t id, std::string shader) {
        this->id = id;
        path = shader;

        debug::print("\n\n--- LOADING SHADER: "+shader+" ---\n");

        // Load shader and create pipeline
        program = loadShader(shader+".wgsl");

        debug::print("RENDER PIPELINE CREATED! - "+std::to_string(program)+"\n");
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
        }
        else {
            throw std::runtime_error("PENUMBRA_ERROR: Couldn't open Shader "+name);
        }

        // Create Render Pipeline for this shader and get the "program"
        uint16_t pipeline = backend::createRenderPipeline(shaderCode);

        return pipeline;
    }
}