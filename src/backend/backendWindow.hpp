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

// For Penumbra Flags
#include "utils/config.hpp"

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>

namespace pen::backend {
    namespace callbacks {
        static void glfw_errorCallback(int error, const char *description) {
            fprintf(stderr, "GLFW error %d: %s\n", error, description);
        }
    }

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
            createWindow();
            // Run backend-specific code
            initBackendSpecific();
            // Create Framebuffers
            createFramebuffers();
        }
        ~BackendWindow() {
            glfwDestroyWindow(window);
            std::cout << "PENUMBRA: Destroyed Window Correctly\n";
        }

        // Returns the time passed since the last time that update() was called.
        double getDeltaTime() { return deltaTime; }

        // After this update ends, the window will be closed
        void closeWindow() { glfwSetWindowShouldClose(window, true); }
        // Returns false if closeWindow() was closed at any point and true otherwise
        bool running() { return !glfwWindowShouldClose(window); }

        // Get 2D Framebuffer. Pass this to the 2D renderer
        uint16_t get2DFramebuffer() { return framebuffer2D; }
        // Get 3D Framebuffer. Pass this to the 3D renderer
        uint16_t get3DFramebuffer() { return framebuffer3D; }

        // TODO: Custom framebuffers
        // Set custom framebuffer by name. Call this function to draw to a custom
        // framebuffer.


        // Set Window Size.
        // Rebuilds framebuffers
        void resize(uint16_t x, uint16_t y);

        // Get Window Size
        uint16_t getWidth() { return width; }
        uint16_t getHeight() { return height; }

        // Shows the framebuffers used (2D and/or 3D) to the screen
        // and polls events
        void update();

    private:
        // Creates the window. This code is common for all backends
        // It only creates the GLFW Window
        void createWindow() {
            // Set glfw error callback
            glfwSetErrorCallback(callbacks::glfw_errorCallback);

            // Set Linux-specific GLFW Init Hints
            // These hints are here to tell us if we should create a X11 or a Wayland window

            // Linux and BSD (BSD was in a BGFX example when I started this project so...)
            // Code for these defines totally not yoinked from BX's platform.h
            #if    defined(__linux__)          \
                || defined(__FreeBSD__)        \
                || defined(__FreeBSD_kernel__) \
                || defined(__NetBSD__)         \
                || defined(__OpenBSD__)        \
                || defined(__DragonFly__)
                // Set the variable indicating if we're using wayland or not to FALSE.
                // By default we'll say that we're on X11
                isWayland = false;

                // If the wayland flag was passed try to create a wayland window
                if (penumbraFlags & PENUMBRA_WAYLAND) {
                    // If Wayland is supported, create the window
                    #ifdef PENUMBRA_WL_COMPAT
                        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
                        isWayland = true; // Tell the program that we're using wayland, not X11
                    #else
                        throw std::runtime_error("PENUMBRA_ERROR: -wayland Argument passed but Wayland support is disabled");
                    #endif
                } else {
                    // If the flag was not passed, try to create an X11 window.
                    #ifdef PENUMBRA_X11_COMPAT
                        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
                    #else
                        throw std::runtime_error("PENUMBRA_ERROR: Tried to create X11 window but X11 support is disabled");
                    #endif
                }
            #endif

            // Init GLFW
            if (!glfwInit()) {
                throw std::runtime_error("PENUMBRA_ERROR: Couldn't initialize GLFW");
            }

            // Set Flags
            if (penumbraFlags & PENUMBRA_TRANSPARENT) {
                glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
                std::cout << "PENUMBRA_WARNING: Transparency Enabled. Bugs WILL happen\n";
            } else {
                // Yep. It's exactly as cursed as it looks like.
                // We don't set this when creating a transparent window.
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            }

            // Create GLFW Window
            window = glfwCreateWindow(width, height, windowTitle.c_str(), NULL, NULL);
            if (!window) {
                throw std::runtime_error("PENUMBRA_ERROR: Couldn't create GLFW Window");
            }
        }

        // Backend Specific code called during window creation.
        // It is called right after createWindow() and should
        // implement GLFW callbacks as well as initialize the
        // rendering backend
        void initBackendSpecific();

        // Creates a framebuffer and stores its ID (the Framebuffer
        // is in GPU memory probably)
        void createFramebuffers();

        // Window State and Information
        GLFWwindow* window;
        std::string windowTitle;
        uint16_t width;
        uint16_t height;
        uint32_t penumbraFlags;
        // Linux-specific
        bool isWayland = false;

        // Framebuffers
        // (views in the case of BGFX)
        uint16_t framebuffer2D = 0;
        uint16_t framebuffer3D = 1;

        // Time
        double lastTime = 0;
        double currentTime = 0;

        double deltaTime = 0;

        // Debug
        bool debugging = false;
    };
}