#include "shader.hpp"

#include <bgfx/bgfx.h>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

namespace pen::antumbra {
    Shader::Shader(uint32_t id, std::string shader) {
        this->id = id;
        path = shader;

        std::cout << "--- LOADING SHADER: "+shader+" ---\n";

        vsh = loadShader(shader+".vs");
        fsh = loadShader(shader+".fs");
        program = bgfx::createProgram(vsh, fsh, true);

        std::cout << "SHADER PROGRAM CREATED! - "+std::to_string(program.idx)+"\n";
        if (!bgfx::isValid(program)) {
            std::cerr << "INVALID SHADER! FUUUUUUUUUUUCKKKK\n";
        }
    }

    bgfx::ShaderHandle Shader::loadShader(std::string name) {
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
            throw std::runtime_error("ANTUMBRA_ERROR: Couldn't open Shader "+name);
        }

        const bgfx::Memory* mem = bgfx::copy(data,fileSize+1);
        mem->data[mem->size-1] = '\0';
        bgfx::ShaderHandle handle = bgfx::createShader(mem);
        bgfx::setName(handle, name.c_str());

        if (bgfx::isValid(handle)) {
            std::cout << "VALID SHADER! ("+name+" = "+std::to_string(handle.idx)+")\n";
        } else {
            std::cerr << "INVALID SHADER! ("+name+" = "+std::to_string(handle.idx)+")\n";
        }

        return handle;
    }
}