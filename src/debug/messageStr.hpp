#pragma once

#include <bgfx/bgfx.h>
#include <cstdint>
#include <string>

#include "debug/consoleColors.hpp"
#include "debug/message.hpp"

namespace pen::debug::log {
    class MessageStr : public Message {
    public:
        MessageStr(std::string msg, uint16_t x, uint16_t y, Color color, Color background) : Message(x, y, color, background) {
            this->msg = msg;
        }

        void print(uint16_t consoleX, uint16_t consoleY) {
            uint16_t posX = x;
            uint16_t posY = y;

            if (alignRight) {
                posX = consoleX - posX;
            }
            if (alignBottom) {
                posY = consoleY - posY;
            }

            bgfx::dbgTextPrintf(posX, posY, color, msg.c_str());
        }

    private:
        std::string msg;
    };
}