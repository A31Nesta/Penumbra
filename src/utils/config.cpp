#include "config.hpp"

namespace pen {
    // Creates an unsigned 32 bit integer with all flags from command line arguments
    uint32_t getFlagsFromArguments(int argc, char** argv) {
        uint32_t flags = 0;

        // Check if flag is valid and put its value in the flags variable
        for (int i = 0; i < argc; i++) {
            if (CLI_INIT_FLAGS.contains(argv[i])) {
                flags = flags | CLI_INIT_FLAGS.at(argv[i]);
            }
        }

        return flags;
    }
}