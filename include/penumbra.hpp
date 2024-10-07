#pragma once

#include <cstdint>
#include <string>

namespace pen {
    void init(std::string title, int width, int height, uint32_t penumbra_flags);
    void update();
    void end();
    
    bool running();
}