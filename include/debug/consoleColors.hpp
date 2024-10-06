#pragma once

#include <cstdint>

namespace pen::debug {
    // All possible colors that can be used to print text
    enum class Color {
        NONE,
        DARK_BLUE,
        DARK_GREEN,
        DARK_AQUA,
        DARK_RED,
        DARK_PURPLE,
        DARK_YELLOW,
        GRAY,
        DARK_GRAY,
        BLUE,
        GREEN,
        AQUA,
        RED,
        PURPLE,
        YELLOW,
        WHITE
    };

    uint8_t colorToForeground(Color color);
    uint8_t colorToBackground(Color color);
}