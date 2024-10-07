#pragma once

/**
    Window class.
    Uses BGFX and GLFW to create the window and context
    ---------------------------------------------------
    
    It provides functions that interact with the underlying GLFW and BGFX stuff
    This class is not responsible for rendering. That's the renderer's job

    This class does hold a pointer to a renderer of each type. It can be 
    created along with the window or set manually. Setting the renderer manually
    allows you to share a renderer between windows for stuff like multi-window
    rendering.

    This doesn't mean that it's supported but the possibility that it gets implemented
    at some point is not 0 either.
*/

#include "core/windowStruct.hpp"
#include <cstdint>
#include <string>

namespace pen::core {
    class Window {
    public:
        Window(std::string title, int width, int height, uint32_t penumbra_flags, uint16_t view3D, uint16_t view2D);
        ~Window();

        // Set behavior properties
        void setWindowCanClose(bool canClose);
        bool windowCanClose();

        // Normal functions
        void update();
        void end();
        bool running();

        void setCurrent();
        WindowStruct& getWinStruct();

    private:
        void init(std::string title, int width, int height, uint32_t penumbra_flags);
        void draw();

	    WindowStruct winStruct;

        // Behavior Properties
        bool canClose = true;
    };
}