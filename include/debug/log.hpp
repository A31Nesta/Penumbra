#pragma once

#include <cstdint>
#include <string>
#include <utility>

#include "consoleColors.hpp"

// TODO: Delete messages that go out of screen
// TODO: In positioned messages, re-use deleted spaces in vector
// TODO: Error handling for when a Value Message loses its reference
namespace pen::debug {
    // Updates the console, this is what actually writes to the screen
    void updateConsole();

    // Set debug log cursor position
    void setCursorPos(uint16_t x, uint16_t y);
    // Get cursor position
    std::pair<uint16_t, uint16_t> getCursorPos();

    // Get Console size
    std::pair<uint16_t, uint16_t> getConsoleSize();

    // Prints the desired text into the screen. It automatically prints the text after the last line
    void print(std::string msg, Color color = Color::WHITE, Color background = Color::NONE);
    // Prints the value of the variable passed by reference. This reference is held onto and updated
    // until the console is cleared
    void printValue(int& variable, Color color = Color::WHITE, Color background = Color::NONE);
    void printValue(long& variable, Color color = Color::WHITE, Color background = Color::NONE);
    void printValue(float& variable, Color color = Color::WHITE, Color background = Color::NONE);
    void printValue(double& variable, Color color = Color::WHITE, Color background = Color::NONE);

    // Print the desired text in a fixed position. This can be used to print labels for value prints
    // It returns the ID of the positioned text. This ID can be used to unregister the text
    uint32_t printPositioned(std::string msg, uint16_t x, uint16_t y, bool alignRight = false, bool alignBottom = false, Color color = Color::WHITE, Color background = Color::NONE);
    // Prints the value of the variable passed by reference in a fixed position. This reference is
    // held onto and updated until the text is unregistered
    // It returns the ID of the positoined text.
    uint32_t printPositionedValue(int& variable, uint16_t x, uint16_t y, bool alignRight = false, bool alignBottom = false, Color color = Color::WHITE, Color background = Color::NONE);
    uint32_t printPositionedValue(long& variable, uint16_t x, uint16_t y, bool alignRight = false, bool alignBottom = false, Color color = Color::WHITE, Color background = Color::NONE);
    uint32_t printPositionedValue(float& variable, uint16_t x, uint16_t y, bool alignRight = false, bool alignBottom = false, Color color = Color::WHITE, Color background = Color::NONE);
    uint32_t printPositionedValue(double& variable, uint16_t x, uint16_t y, bool alignRight = false, bool alignBottom = false, Color color = Color::WHITE, Color background = Color::NONE);
    
    // Deletes a Positioned text
    void deletePositioned(uint32_t id);

    // Clears the console without unregistering positioned texts
    void clearConsole();
    // Clears the console and unregisters all positioned texts of all types
    void clearAll();
}