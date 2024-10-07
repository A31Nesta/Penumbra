#pragma once

#include <cstdint>
#include <string>

namespace pen::core {
    uint32_t createWindow(std::string title, int width, int height, uint32_t flags);
    void setWindowCloseable(uint32_t window, bool closeable);

    void updateWindows();
    void closeWindow(uint32_t window);
    void closeAll();

    bool running();
}