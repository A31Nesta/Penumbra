#pragma once

#include "antumbra/antumbra.hpp"
#include "core/window.hpp"
#include <cstdint>
#include <string>

namespace pen {
    core::Window* init(std::string title, int width, int height, uint32_t penumbra_flags);
    void update();
    void end();
    
    bool running();

    core::Window* getWindow();
    antumbra::Antumbra* getAntumbra();
}