#pragma once

#include <cstdint>
#include <unordered_map>
#include <string>

#define PENUMBRA_WAYLAND        UINT32_C(1)
#define PENUMBRA_TRANSPARENT    UINT32_C(2)

namespace pen {
    // Flags passed to the program to indicate how to create the window
    const std::unordered_map<std::string, uint32_t> CLI_INIT_FLAGS = {
        {"-wayland", PENUMBRA_WAYLAND}, // Linux only, it forces Penumbra to create a Wayland window instead of X11
        {"-transparent", PENUMBRA_TRANSPARENT} // Creates a transparent window. Experimental and most likely unstable
    };

    // Creates an unsigned 32 bit integer with all flags from command line arguments
    uint32_t getFlagsFromArguments(int argc, char** argv);
}