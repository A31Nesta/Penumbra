#include "debug/consoleColors.hpp"

namespace pen::debug {
    uint8_t colorToForeground(Color color) {
        return colorToBackground(color) >> 4;
    }
    uint8_t colorToBackground(Color color) {
        uint8_t c;

        switch (color) {
        case Color::NONE:
            c = 0x00;
            break;
        case Color::DARK_BLUE:
            c = 0x10;
            break;
        case Color::DARK_GREEN:
            c = 0x20;
            break;
        case Color::DARK_AQUA:
            c = 0x30;
            break;
        case Color::DARK_RED:
            c = 0x40;
            break;
        case Color::DARK_PURPLE:
            c = 0x50;
            break;
        case Color::DARK_YELLOW:
            c = 0x60;
            break;
        case Color::GRAY:
            c = 0x70;
            break;
        case Color::DARK_GRAY:
            c = 0x80;
            break;
        case Color::BLUE:
            c = 0x90;
            break;
        case Color::GREEN:
            c = 0xa0;
            break;
        case Color::AQUA:
            c = 0xb0;
            break;
        case Color::RED:
            c = 0xc0;
            break;
        case Color::PURPLE:
            c = 0xd0;
            break;
        case Color::YELLOW:
            c = 0xe0;
            break;
        case Color::WHITE:
            c = 0xf0;
            break;
        }

        return c;
    }
};