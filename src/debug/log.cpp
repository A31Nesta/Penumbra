#include "debug/log.hpp"
#include "debug/message.hpp"
#include "debug/messageStr.hpp"
#include "debug/messageNum.hpp"

#include <algorithm>
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

        // Debug
        int _debx = 0;
        int _deby = 0;
        int _maxx = 0;
        int _maxy = 0;
        int _logSize = 0;
        bool _debuggingPosition = false;

        // Messages
        std::vector<log::Message*> _consoleLog; // All the non-positioned messages
        std::vector<log::Message*> _positionedLog; // All the positioned messages
    }

    // Updates the console, this is what actually writes to the screen
    void updateConsole() {
        // Get Console size
        std::pair<uint16_t, uint16_t> cSize = getConsoleSize();

        if (_debuggingPosition) {
            _debx = _x;
            _deby = _y;
            _maxx = cSize.first;
            _maxy = cSize.second;
            _logSize = _consoleLog.size();
        }

        // Clear
        bgfx::dbgTextClear();

        // Console Log
        for (log::Message* msg : _consoleLog) {
            if (msg == nullptr) {
                continue;
            }
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

    // UTILITIES (PRIVATE)
    
    // Moves all messages down (or up if it's a negative number)
    void moveAll(int spaces) {
        for (uint32_t i = 0; i < _consoleLog.size(); i++) {
            log::Message* msg = _consoleLog.at(i);
            if (msg == nullptr) {
                continue;
            }

            msg->translate(0, spaces);
            if (msg->getPos().second<0) {
                delete msg;
                _consoleLog.at(i) = nullptr;
            }
        }
    }

    // Checks the position of the text cursor and moves everything up
    void correctTextView() {
        std::pair<uint16_t, uint16_t> consoleSize = getConsoleSize();
        uint16_t offset = std::max(_y - consoleSize.second, 0);
        if (offset) {
            moveAll(-offset);
            _y = consoleSize.second;
        }
    }
    // END UTILITIES

    // Prints the desired text into the screen. It automatically prints the text after the last line
    void print(std::string msg, Color color, Color background) {
        std::stringstream ss(msg);
        std::string line;

        // Original Coordinates. Saved because we don't necessarily want to go to the next line
        uint16_t origX = _x;
        uint16_t origY = _y;

        while (std::getline(ss, line, '\n')) {
            log::MessageStr* message = new log::MessageStr(line, _x, _y, color, background);

            // Find free space on vector
            uint32_t id = 0;
            bool idFound = false;
            for (uint32_t i = 0; i < _consoleLog.size(); i++) {
                if (_consoleLog.at(i)==nullptr) {
                    id = i;
                    idFound = true;
                    break;
                }
            }
            // Put message in free space, if there's no free space create a new space
            if (idFound) {
                _consoleLog.at(id) = message;
            } else {
                _consoleLog.push_back(message);
            }

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

        correctTextView();
    }
    // Prints the value of the variable passed by reference. This reference is held onto and updated
    // until the console is cleared
    void printValue(int& variable, Color color, Color background) {
        log::MessageInt* msg = new log::MessageInt(variable, _x, _y, color, background);
        _consoleLog.push_back(msg);
        // Set the coords to the next line
        _x = 0;
        _y += 1;

        correctTextView();
    }
    void printValue(long& variable, Color color, Color background) {
        log::MessageLong* msg = new log::MessageLong(variable, _x, _y, color, background);
        _consoleLog.push_back(msg);
        // Set the coords to the next line
        _x = 0;
        _y += 1;

        correctTextView();
    }
    void printValue(float& variable, Color color, Color background) {
        log::MessageFloat* msg = new log::MessageFloat(variable, _x, _y, color, background);
        _consoleLog.push_back(msg);
        // Set the coords to the next line
        _x = 0;
        _y += 1;

        correctTextView();
    }
    void printValue(double& variable, Color color, Color background) {
        log::MessageDouble* msg = new log::MessageDouble(variable, _x, _y, color, background);
        _consoleLog.push_back(msg);
        // Set the coords to the next line
        _x = 0;
        _y += 1;

        correctTextView();
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

    // DEBUG
    void debugCursorPosition() {
        _debuggingPosition = true;

        printPositioned("CUR", 3, 0, true);
        printPositionedValue(_deby, 7, 0, true);
        printPositionedValue(_debx, 13, 0, true);


        printPositioned("MAX", 3, 1, true);
        printPositionedValue(_maxy, 7, 1, true);
        printPositionedValue(_maxx, 13, 1, true);

        printPositioned("LOG", 3, 2, true);
        printPositionedValue(_logSize, 7, 2, true);
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