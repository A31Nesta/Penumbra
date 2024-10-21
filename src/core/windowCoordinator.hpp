#pragma once

#include "core/window.hpp"
#include <cstdint>
#include <string>

namespace pen::core {
    uint32_t createWindow(std::string title, int width, int height, uint32_t flags);
    void setWindowCloseable(uint32_t window, bool closeable);

    // Create or set renderers for windows
    void createAntumbraForWindow(uint32_t window, std::string defaultShader);

    // Get Window
    Window* getWindow(uint32_t window);

    void updateWindows();
    void closeWindow(uint32_t window);
    void closeAll();

    bool running();
}