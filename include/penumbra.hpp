#pragma once

#include "core/window.hpp"
#include <cstdint>
#include <string>

namespace pen {
    void init(std::string title, int width, int height, uint32_t penumbra_flags);
    void update();
    void end();
    
    bool running();

    core::Window* getWindow(uint32_t window = 0);
}