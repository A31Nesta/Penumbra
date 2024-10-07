#include "debug/log.hpp"
#include "penumbra.hpp"
#include "utils/config.hpp"

#include <bgfx/bgfx.h>
#include <cstdint>
#include <memory>
#include <array>

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

int main(int argc, char** argv) {
    uint32_t flags = pen::getFlagsFromArguments(argc, argv);
    pen::init("Penumbra", 1280, 720, flags);

    // Set Debug Stuff
    pen::debug::print(" PENUMBRA ", pen::debug::Color::DARK_GRAY, pen::debug::Color::WHITE);
    pen::debug::print(" -  Debug Version \n\n", pen::debug::Color::WHITE, pen::debug::Color::DARK_GRAY);
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

    pen::debug::print(exec("cowsay mooooo"));

    // Run Program
    while (pen::running()) {
        counter++;
        pen::update();
    }

    pen::end();
}