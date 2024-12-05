#pragma once
#include <cstdint>
#include <string>

namespace pen {
    class Shader {
    public:
        Shader(uint32_t id, std::string shader);
        ~Shader();

        void incrementUsers() { users++; }
        void decrementUsers() { users--; }
        void setPersistence(bool pers) { persistent = pers; }

        std::string getPath() { return path; }
        uint32_t getID() { return id; }
        uint16_t getProgram() { return program; }

    private:
        // BGFX's ShaderHandle
        uint16_t loadShader(std::string name);

        // BGFX's ProgramHandle
        uint16_t program;

        std::string path;
        uint32_t users = 1; // Initialized to 1 because at the time of creation 1 object would be using it
        bool persistent = false;

        uint32_t id;
    };
}