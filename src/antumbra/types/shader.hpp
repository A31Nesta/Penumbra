#pragma once

#include <bgfx/bgfx.h>
#include <cstdint>
#include <string>
// #include <vector>

namespace pen {
    class Shader {
    public:
        Shader(uint32_t id, std::string shader);

        // void createUniform(std::string name, bgfx::UniformType::Enum type);

        void incrementUsers() { users++; }
        void decrementUsers() { users--; }
        void setPersistence(bool pers) { persistent = pers; }

        std::string getPath() { return path; }
        uint32_t getID() { return id; }
        bgfx::ProgramHandle getProgram() { return program; }

    private:
        bgfx::ShaderHandle loadShader(std::string name);

        // BGFX Shader things
        bgfx::ShaderHandle vsh;
        bgfx::ShaderHandle fsh;
        bgfx::ProgramHandle program;

        // Uniforms
        // std::vector<bgfx::UniformHandle> localUniforms;

        std::string path;
        uint32_t users = 1; // Initialized to 1 because at the time of creation 1 object would be using it
        bool persistent = false;

        uint32_t id;
    };
}