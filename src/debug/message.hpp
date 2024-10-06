#pragma once

#include <bgfx/bgfx.h>
#include <cstdint>

#include "debug/consoleColors.hpp"

namespace pen::debug::log {
    class Message {
    public:
        Message(uint16_t x, uint16_t y, Color color, Color background) {
            this->x = x;
            this->y = y;

            // convert Colors to 4 bit colors
            uint8_t fg = colorToForeground(color);
            uint8_t bg = colorToBackground(background);

            // Put the two colors together
            this->color = bg | fg;
        }
        virtual ~Message() {};

        void setAlign(bool right, bool bottom) {
            alignRight = right;
            alignBottom = bottom;
        }

        virtual void print(uint16_t consoleX, uint16_t consoleY) = 0;

        void translate(uint16_t x, uint16_t y) {
            this->x += x;
            this->y += y;
        }
        void setPos(uint16_t x, uint16_t y) {
            this->x = x;
            this->y = y;
        }

    protected:
        uint16_t x;
        uint16_t y;

        bool alignRight = false;
        bool alignBottom = false;

        uint8_t color;
    };
}