#pragma once

/**
    Backend Window
    --------------

    Creates a native window using a backend.
    Doesnt't include renderer pointers; the Window class in src/core/window.cpp has
    to handle the renderers, as this class is just a window and its events.

    It guarantees that you can use 2 framebuffers: a main one and a UI one.
    They're composed into a third framebuffer that is shown to the screen

    - Drawing a scene to a custom framebuffer is possible.
    - Drawing debug information may also involve another framebuffer
*/

#include <GLFW/glfw3.h>

#include <cstdint>
#include <string>

class BackendWindow {
public:
    // Construct the window with a title, width, height and flags
    BackendWindow(std::string title, uint16_t width, uint16_t height, uint32_t penumbraFlags) {
        // Save to Window State and Information
        this->windowTitle = title;
        this->width = width;
        this->height = height;
        this->penumbraFlags = penumbraFlags;

        // Create Window
        init(title, width, height, penumbraFlags);

        // Create Framebuffers
        // TODO: Create backend-independent Framebuffer Objects
    }
    ~BackendWindow();

    // Returns the time passed since the last time that update() was called.
    double getDeltaTime() { return deltaTime; }

    // After this update ends, the window will be closed
    void closeWindow() { isRunning = false; }
    // Returns false if closeWindow() was closed at any point and true otherwise
    bool running() { return isRunning; }

    // Set Main Framebuffer. Draw the scene after calling this function
    void setMainFramebuffer();
    // Set Overlay Framebuffer. Draw the UI after calling this function
    void setOverlayFramebuffer();

    // TODO: Custom framebuffers
    // Set custom framebuffer by name. Call this function to draw to a custom
    // framebuffer.

    // Shows the framebuffers used (main and/or overlay) to the screen
    // and polls events
    void update();

private:
    // Creates the window
    void init(std::string title, uint16_t width, uint16_t height, uint32_t penumbraFlags);

    // Window State and Information
    GLFWwindow* window;
    std::string windowTitle;
    uint16_t width;
    uint16_t height;
    uint32_t penumbraFlags;

    // Is set to false when closeWindow() is called
    bool isRunning = true;

    // Time
    double deltaTime = 0;
};