#include "debug/log.hpp"
#include "debug/message.hpp"
#include "debug/messageStr.hpp"
#include "debug/messageNum.hpp"

#include <bgfx/bgfx.h>
#include <cstdint>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace pen::debug {
    // Console Data
    namespace {
        uint16_t _x = 0;
        uint16_t _y = 0;

        std::vector<log::Message*> _consoleLog; // All the non-positioned messages
        std::vector<log::Message*> _positionedLog; // All the positioned messages
    }

    // Updates the console, this is what actually writes to the screen
    void updateConsole() {
        // Get Console size
        std::pair<uint16_t, uint16_t> cSize = getConsoleSize();

        // Clear
        bgfx::dbgTextClear();

        // Console Log
        for (log::Message* msg : _consoleLog) {
            msg->print(cSize.first, cSize.second);
        }

        // Positioned Text
        for (log::Message* msg : _positionedLog) {
            if (msg == nullptr) {
                continue;
            }
            msg->print(cSize.first, cSize.second);
        }
    }

    // Set debug log cursor position
    void setCursorPos(uint16_t x, uint16_t y) {
        _x = x;
        _y = y;
    }
    // Get cursor position
    std::pair<uint16_t, uint16_t> getCursorPos() {
        return {_x, _y};
    }

    // Get Console size
    std::pair<uint16_t, uint16_t> getConsoleSize() {
        const bgfx::Stats* stats = bgfx::getStats();
        return {stats->textWidth, stats->textHeight};
    }

    // Prints the desired text into the screen. It automatically prints the text after the last line
    void print(std::string msg, Color color, Color background) {
        std::stringstream ss(msg);
        std::string line;

        // Original Coordinates. Saved because we don't necessarily want to go to the next line
        uint16_t origX = _x;
        uint16_t origY = _y;

        while (std::getline(ss, line, '\n')) {
            log::MessageStr* message = new log::MessageStr(line, _x, _y, color, background);
            _consoleLog.push_back(message);

            // Save updated coords. The length of the line must be added to X
            origX = _x + line.length();
            origY = _y;

            // Set the coords to the next line
            _x = 0;
            _y += 1;
        }

        // If the message didn't end with \n we restore the previous coordinates
        if (!msg.ends_with('\n')) {
            _x = origX;
            _y = origY;
        }
    }
    // Prints the value of the variable passed by reference. This reference is held onto and updated
    // until the console is cleared
    void printValue(int& variable, Color color, Color background) {
        log::MessageInt* msg = new log::MessageInt(variable, _x, _y, color, background);
        _consoleLog.push_back(msg);
        // Set the coords to the next line
        _x = 0;
        _y += 1;
    }
    void printValue(long& variable, Color color, Color background) {
        log::MessageLong* msg = new log::MessageLong(variable, _x, _y, color, background);
        _consoleLog.push_back(msg);
        // Set the coords to the next line
        _x = 0;
        _y += 1;
    }
    void printValue(float& variable, Color color, Color background) {
        log::MessageFloat* msg = new log::MessageFloat(variable, _x, _y, color, background);
        _consoleLog.push_back(msg);
        // Set the coords to the next line
        _x = 0;
        _y += 1;
    }
    void printValue(double& variable, Color color, Color background) {
        log::MessageDouble* msg = new log::MessageDouble(variable, _x, _y, color, background);
        _consoleLog.push_back(msg);
        // Set the coords to the next line
        _x = 0;
        _y += 1;
    }

    // Print the desired text in a fixed position. This can be used to print labels for value prints
    // It returns the ID of the positioned text. This ID can be used to unregister the text
    uint32_t printPositioned(std::string msg, uint16_t x, uint16_t y, bool alignRight, bool alignBottom, Color color, Color background) {
        log::MessageStr* message = new log::MessageStr(msg, x, y, color, background);
        message->setAlign(alignRight, alignBottom);
        _positionedLog.push_back(message);
        return _positionedLog.size()-1;
    }
    // Prints the value of the variable passed by reference in a fixed position. This reference is
    // held onto and updated until the text is unregistered
    // It returns the ID of the positoined text.
    uint32_t printPositionedValue(int& variable, uint16_t x, uint16_t y, bool alignRight, bool alignBottom, Color color, Color background) {
        log::MessageInt* msg = new log::MessageInt(variable, x, y, color, background);
        msg->setAlign(alignRight, alignBottom);
        _positionedLog.push_back(msg);
        return _positionedLog.size()-1;
    }
    uint32_t printPositionedValue(long& variable, uint16_t x, uint16_t y, bool alignRight, bool alignBottom, Color color, Color background) {
        log::MessageLong* msg = new log::MessageLong(variable, x, y, color, background);
        msg->setAlign(alignRight, alignBottom);
        _positionedLog.push_back(msg);
        return _positionedLog.size()-1;
    }
    uint32_t printPositionedValue(float& variable, uint16_t x, uint16_t y, bool alignRight, bool alignBottom, Color color, Color background) {
        log::MessageFloat* msg = new log::MessageFloat(variable, x, y, color, background);
        msg->setAlign(alignRight, alignBottom);
        _positionedLog.push_back(msg);
        return _positionedLog.size()-1;
    }
    uint32_t printPositionedValue(double& variable, uint16_t x, uint16_t y, bool alignRight, bool alignBottom, Color color, Color background) {
        log::MessageDouble* msg = new log::MessageDouble(variable, x, y, color, background);
        msg->setAlign(alignRight, alignBottom);
        _positionedLog.push_back(msg);
        return _positionedLog.size()-1;
    }

    // Deletes a Positioned text
    void deletePositioned(uint32_t id) {
        delete _positionedLog.at(id);
        _positionedLog.at(id) = nullptr;
    }

    // Clears the console without unregistering positioned texts
    void clearConsole() {
        for (log::Message* msg : _consoleLog) {
            delete msg;
        }
        _consoleLog.clear();
    }
    // Clears the console and unregisters all positioned texts of all types
    void clearAll() {
        clearConsole();
        for (log::Message* msg : _positionedLog) {
            delete msg;
        }
        _positionedLog.clear();
    }
}