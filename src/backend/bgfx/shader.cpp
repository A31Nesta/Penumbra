#include "../shader.hpp"
#include "debug/consoleColors.hpp"
#include "debug/log.hpp"

#include <bgfx/bgfx.h>
#include <fstream>
#include <stdexcept>
#include <string>

namespace pen {
    Shader::Shader(uint32_t id, std::string shader) {
        this->id = id;
        path = shader;

        debug::print("\n\n--- LOADING SHADER: "+shader+" ---\n");

        bgfx::ShaderHandle vsh = (bgfx::ShaderHandle)loadShader(shader+".vs");
        bgfx::ShaderHandle fsh = (bgfx::ShaderHandle)loadShader(shader+".fs");
        program = bgfx::createProgram(vsh, fsh, false).idx;
        
        // Destroy shaders (not program) immediately
        bgfx::destroy(vsh);
        bgfx::destroy(fsh);

        debug::print("SHADER PROGRAM CREATED! - "+std::to_string(program)+"\n");
        if (!bgfx::isValid((bgfx::ProgramHandle)program)) {
            debug::print("INVALID SHADER! FUUUUUUUUUUUCKKKK\n", debug::Color::WHITE, debug::Color::DARK_RED);
        }
    }
    Shader::~Shader() {
        // Delete program
        bgfx::destroy((bgfx::ProgramHandle)program);
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

        const bgfx::Memory* mem = bgfx::copy(data,fileSize+1);
        mem->data[mem->size-1] = '\0';
        bgfx::ShaderHandle handle = bgfx::createShader(mem);
        bgfx::setName(handle, name.c_str());

        if (bgfx::isValid(handle)) {
            debug::print("VALID SHADER! ("+name+" = "+std::to_string(handle.idx)+")\n");
        } else {
            debug::print("INVALID SHADER! ("+name+" = "+std::to_string(handle.idx)+")\n", debug::Color::WHITE, debug::Color::DARK_RED);
        }

        return handle.idx;
    }
}