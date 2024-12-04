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
#include "backend/backendWindow.hpp"
#include <cstdint>
#include <string>

namespace pen::core {
    class Window {
    public:
        Window(std::string title, int width, int height, uint32_t penumbraFlags);
        ~Window();

        // Tells the window to close after this update is done
        void closeWindow();
        // Returns false if closeWindow() was closed at any point and true otherwise
        bool running();

        // Create Renderers
        void createAntumbra(std::string defaultShader);

        // Access Renderers
        antumbra::Antumbra* getAntumbra() { return antumbra; }

        // Update window and draw
        void update();

        // Gets delta time from window
        double getDeltaTime();

    private:
        void draw();

        // Backend Window
        backend::BackendWindow window;

        // Renderers
        antumbra::Antumbra* antumbra;
    };
}