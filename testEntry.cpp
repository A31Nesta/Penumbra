#include "debug/log.hpp"
#include "penumbra.hpp"
#include "utils/config.hpp"

#include <bgfx/bgfx.h>
#include <cstdint>

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

    pen::debug::print("Please put an NSFW tag on this.\nI was on the train and when I saw this I had to start furiously masturbating.\n"
        "Everyone else gave me strange looks and were saying things like \"what the fuck\" and \"call the police\".\nI dropped my phone and everyone around me saw this image.\n"
        "Now there is a whole train of men masturbating together at this one image.\nThis is all your fault, you could have prevented this if you had just tagged this post NSFW.\n");

    // Run Program
    while (pen::running()) {
        counter++;
        pen::update();
    }

    pen::end();
}