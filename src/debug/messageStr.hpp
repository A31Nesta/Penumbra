#pragma once

#include <cstdint>
#include <string>

#include "debug/consoleColors.hpp"
#include "debug/message.hpp"

// HACK: Disable Debug log when not using BGFX
#ifdef PENUMBRA_BACKEND_BGFX
    #include <bgfx/bgfx.h>
#endif
namespace pen::debug::log {
    class MessageStr : public Message {
    public:
        MessageStr(std::string msg, int16_t x, int16_t y, Color color, Color background) : Message(x, y, color, background) {
            this->msg = msg;
        }

        void print(uint16_t consoleX, uint16_t consoleY) {
            int16_t posX = x;
            int16_t posY = y;

            if (alignRight) {
                posX = consoleX - posX;
            }
            if (alignBottom) {
                posY = consoleY - posY;
            }

            #ifdef PENUMBRA_BACKEND_BGFX
                bgfx::dbgTextPrintf(posX, posY, color, msg.c_str());
            #endif
        }

    private:
        std::string msg;
    };
}