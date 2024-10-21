#include "antumbra/antumbra.hpp"
#include "core/window.hpp"
#include "debug/log.hpp"
#include "penumbra.hpp"
#include "utils/config.hpp"
#include "utils/vectors.hpp"

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
    pen::init("Penumbra", 1280, 720, flags);

    pen::core::Window* window = pen::getWindow();
    pen::antumbra::Antumbra* antumbra = window->getAntumbra();

    pen::antumbra::Sprite* sun = antumbra->addSprite("pnmbr/images/hexta.ktx", pen::Vec2(0, 0), 0, 0.5);
    pen::antumbra::Sprite* planet = antumbra->addSprite("pnmbr/images/sus.png", pen::Vec2(0.5, 0), 0, 0.125);
    pen::antumbra::Sprite* planet2 = antumbra->addSprite("pnmbr/images/sus.png", pen::Vec2(0, 0.75), 0, 0.25);

    pen::antumbra::Sprite* moongus = antumbra->addSprite("pnmbr/images/amogus.png", pen::Vec2(-0.5, 0), 0, 0.125);

    putDebugShit(flags);

    // Planet Forces
    pen::Vec2 force(0, 0);
    pen::Vec2 velocity(0, 1.0);

    pen::Vec2 force2(0, 0);
    pen::Vec2 velocity2(-1.2, 0);

    pen::Vec2 force3(0, 0);
    pen::Vec2 velocity3(0, -1.0);

    // Run Program
    while (pen::running()) {
        const double dt = window->getDeltaTime();
        sun->transform.rotation += dt * 0.5;
        planet->transform.rotation -= dt;
        planet2->transform.rotation -= dt * 2.0;
        moongus->transform.rotation -= dt * 4.0;
        
        // Calculate Forces
        force = pen::Vec2(0, 0) - planet->transform.position;
        velocity = velocity + (force * 5.0 * dt);
        planet->transform.position = planet->transform.position + (velocity*dt);

        force2 = pen::Vec2(0, 0) - planet2->transform.position;
        velocity2 = velocity2 + (force2 * dt);
        planet2->transform.position = planet2->transform.position + (velocity2*dt);

        force3 = pen::Vec2(0, 0) - moongus->transform.position;
        velocity3 = velocity3 + (force3 * 2.0 * dt);
        moongus->transform.position = moongus->transform.position + (velocity3*dt);

        pen::update();
    }

    pen::end();
}