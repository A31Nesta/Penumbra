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
        MessageInt(int& msg, int16_t x, int16_t y, Color color, Color background) : Message(x, y, color, background), msg(msg) {
            
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

            bgfx::dbgTextPrintf(posX, posY, color, std::to_string(msg).c_str());
        }

    private:
        int& msg;
    };

    // LONG
    class MessageLong : public Message {
    public:
        MessageLong(long& msg, int16_t x, int16_t y, Color color, Color background) : Message(x, y, color, background), msg(msg) {
            
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

            bgfx::dbgTextPrintf(posX, posY, color, std::to_string(msg).c_str());
        }

    private:
        long& msg;
    };

    // FLOAT
    class MessageFloat : public Message {
    public:
        MessageFloat(float& msg, int16_t x, int16_t y, Color color, Color background) : Message(x, y, color, background), msg(msg) {
            
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

            bgfx::dbgTextPrintf(posX, posY, color, std::to_string(msg).c_str());
        }

    private:
        float& msg;
    };
    
    // DOUBLE
    class MessageDouble : public Message {
    public:
        MessageDouble(double& msg, int16_t x, int16_t y, Color color, Color background) : Message(x, y, color, background), msg(msg) {
            
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

            bgfx::dbgTextPrintf(posX, posY, color, std::to_string(msg).c_str());
        }

    private:
        double& msg;
    };
}