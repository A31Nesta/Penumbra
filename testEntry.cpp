#include "debug/log.hpp"
#include "penumbra.hpp"
#include "utils/config.hpp"

#include <bgfx/bgfx.h>
#include <cstdint>
#include <stdexcept>
#include <string>

// Run command and get output.
// When the shell is sus
std::string exec(std::string cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (fgets(buffer, sizeof buffer, pipe) != NULL) 
            result += buffer;
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

int main(int argc, char** argv) {
    uint32_t flags = pen::getFlagsFromArguments(argc, argv);
    pen::init("Penumbra", 1280, 720, flags);

    // Set Debug Stuff
    pen::debug::printPositioned(" RENDERER ", 0, 0, false, false, pen::debug::Color::DARK_GRAY, pen::debug::Color::WHITE);
    pen::debug::printPositioned(" -  Debug Version ", 10, 0, false, false, pen::debug::Color::WHITE, pen::debug::Color::DARK_GRAY);
    pen::debug::setCursorPos(0, 2);
    // EXTRA DEBUGGING INFO
    if (flags & PENUMBRA_TRANSPARENT) {
        pen::debug::printPositioned(" TRANSPARENCY ENABLED: PREPARE FOR BUGS ", 40, 1, true, true, pen::debug::Color::DARK_GRAY, pen::debug::Color::YELLOW);
    }
    #if __linux__
        std::string renderer = bgfx::getRendererName(bgfx::getRendererType());
        if (flags & PENUMBRA_WAYLAND) {
            pen::debug::printPositioned(" Wayland - "+renderer+" ", 0, 1, false, true, pen::debug::Color::WHITE, pen::debug::Color::DARK_BLUE);
        } else {
            pen::debug::printPositioned(" Default - "+renderer+" ", 0, 1, false, true, pen::debug::Color::WHITE, pen::debug::Color::DARK_BLUE);
        }
    #endif
    // Frame Counter
    long counter = 0;
    pen::debug::printPositioned("Frame: ", 0, 2, false, true, pen::debug::Color::YELLOW, pen::debug::Color::DARK_GRAY);
    pen::debug::printPositionedValue(counter, 7, 2, false, true, pen::debug::Color::YELLOW, pen::debug::Color::DARK_GRAY);

    pen::debug::debugCursorPosition();

    // Run Program
    while (pen::running()) {
        counter++;

        if (counter % 500 == 0) {
            // Test without icles for real for real no cap
            pen::debug::print(std::to_string(counter)+"\n");
        }
        
        pen::update();
    }

    pen::end();
}