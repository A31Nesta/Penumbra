#pragma once

/**
    Window class.
    Uses the Backend Interface to create the window and context
    -----------------------------------------------------------
    
    It provides functions that interact with the backend to create a cross-platform window
    This class is not responsible for rendering. That's the renderer's job

    This class holds a pointer to a renderer of each type. It can be 
    created along with the window or set manually. Setting the renderer manually
    allows you to share a renderer between windows for stuff like multi-window
    rendering. (Not planned for now)
*/

#include "antumbra/antumbra.hpp"
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

        // Create or set renderers
        void createAntumbra(std::string defaultShader);

        // Access Renderers
        antumbra::Antumbra* getAntumbra() { return antumbra; }

        // Normal functions
        void update();
        void end();
        bool running();

        double getDeltaTime() { return deltaTime; }

        void setCurrent();
        WindowStruct& getWinStruct();

    private:
        void init(std::string title, int width, int height, uint32_t penumbra_flags);
        void draw();

	    WindowStruct winStruct;

        // Time
        double lastTime = 0;
        double currentTime = 0;

        // Important Stuff
        double deltaTime = 0;

        // Renderers
        antumbra::Antumbra* antumbra;

        // Behavior Properties
        bool canClose = true;
    };
}