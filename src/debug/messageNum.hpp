#pragma once

#include <bgfx/bgfx.h>
#include <cstdint>
#include <string>

#include "debug/consoleColors.hpp"
#include "debug/message.hpp"

namespace pen::debug::log {
    // INT
    class MessageInt : public Message {
    public:
        MessageInt(int& msg, uint16_t x, uint16_t y, Color color, Color background) : Message(x, y, color, background), msg(msg) {
            
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

            bgfx::dbgTextPrintf(posX, posY, color, std::to_string(msg).c_str());
        }

    private:
        int& msg;
    };

    // LONG
    class MessageLong : public Message {
    public:
        MessageLong(long& msg, uint16_t x, uint16_t y, Color color, Color background) : Message(x, y, color, background), msg(msg) {
            
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

            bgfx::dbgTextPrintf(posX, posY, color, std::to_string(msg).c_str());
        }

    private:
        long& msg;
    };

    // FLOAT
    class MessageFloat : public Message {
    public:
        MessageFloat(float& msg, uint16_t x, uint16_t y, Color color, Color background) : Message(x, y, color, background), msg(msg) {
            
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

            bgfx::dbgTextPrintf(posX, posY, color, std::to_string(msg).c_str());
        }

    private:
        float& msg;
    };
    
    // DOUBLE
    class MessageDouble : public Message {
    public:
        MessageDouble(double& msg, uint16_t x, uint16_t y, Color color, Color background) : Message(x, y, color, background), msg(msg) {
            
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

            bgfx::dbgTextPrintf(posX, posY, color, std::to_string(msg).c_str());
        }

    private:
        double& msg;
    };
}