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

void putDebugShit(uint32_t flags) {
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
    
    pen::debug::debugCursorPosition();
}

int main(int argc, char** argv) {
    uint32_t flags = pen::getFlagsFromArguments(argc, argv);
    pen::core::Window* window = pen::init("Penumbra", 1280, 720, flags);

    pen::antumbra::Antumbra* antumbra = pen::getAntumbra();

    pen::antumbra::Sprite* magic_1 = pen::createSprite("pnmbr/images/hexta.ktx", pen::Vec2(0, 0), 0, 0.6);
    pen::antumbra::Sprite* magic = pen::createSprite("pnmbr/images/hexta.ktx", pen::Vec2(0, 0), 0, 1.125);
    pen::antumbra::Sprite* magic2 = pen::createSprite("pnmbr/images/hexta.ktx", pen::Vec2(0, 0), 0, 2);
    pen::antumbra::Sprite* magic3 = pen::createSprite("pnmbr/images/hexta.ktx", pen::Vec2(0, 0), 0, 3.5);

    putDebugShit(flags);

    // Run Program
    while (pen::running()) {
        const double dt = pen::getDeltaTime();
        magic_1->transform.rotation += dt * 1.0;
        magic->transform.rotation -= dt * 0.75;
        magic2->transform.rotation += dt * 0.5;
        magic3->transform.rotation -= dt * 0.25;

        pen::update();
    }

    pen::end();
}