#pragma once

#include <cstdint>
#include <utility>

#include "debug/consoleColors.hpp"

namespace pen::debug::log {
    class Message {
    public:
        Message(int16_t x, int16_t y, Color color, Color background) {
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

        void translate(int16_t x, int16_t y) {
            this->x += x;
            this->y += y;
        }
        void setPos(int16_t x, int16_t y) {
            this->x = x;
            this->y = y;
        }
        std::pair<int16_t, int16_t> getPos() {
            return {x, y};
        }

    protected:
        int16_t x;
        int16_t y;

        bool alignRight = false;
        bool alignBottom = false;

        uint8_t color;
    };
}