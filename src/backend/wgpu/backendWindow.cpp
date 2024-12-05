#include "../backendWindow.hpp"

#include <GLFW/glfw3.h>

#include <cmath>
#include <cstdint>
#include <cstdlib>
#if BX_PLATFORM_LINUX
	// X11 Support
    #ifdef PENUMBRA_X11_COMPAT
		#define GLFW_EXPOSE_NATIVE_X11
	#endif
	// Wayland Support
	#ifdef PENUMBRA_WL_COMPAT
		#include <wayland-egl.h>
		#define GLFW_EXPOSE_NATIVE_WAYLAND
	#endif
#elif BX_PLATFORM_WINDOWS
    #define GLFW_EXPOSE_NATIVE_WIN32
#elif BX_PLATFORM_OSX
    #define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>

/**
    BGFX Implementation of Backend Window
    -------------------------------------

    All the BGFX Code related to window creation and handling goes here  
*/

namespace pen::backend {
    namespace callbacks {
        static void glfw_framebufferResizeCallback(GLFWwindow* window, int width, int height) {
            backend::BackendWindow* win = (backend::BackendWindow*)glfwGetWindowUserPointer(window);
            win->resize(width, height);
        }
    }

    // WGPU Global Variables
    // ---------------------


    // PUBLIC:

    // Shows the framebuffers used (2D and/or 3D) to the screen
    // and polls events
    void BackendWindow::update() {
		// Update Code Here...

        currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
    }

    // Set Window Size.
    // Rebuilds framebuffers
    void BackendWindow::resize(uint16_t x, uint16_t y) {
        width = x;
        height = y;

        // Code called when resizing...
        
        createFramebuffers();
    }

    // PRIVATE:

    void BackendWindow::initBackendSpecific() {
        // Set GLFW window resize callback
        glfwSetFramebufferSizeCallback(window, callbacks::glfw_framebufferResizeCallback);
        // TODO: Set GLFW Input Callbacks
        // Set GLFW Input callbacks

        // Init WGPU Here...

        // Linux and BSD (BSD was in a BGFX example when I started this project so...)
        // Code for these defines totally not yoinked from BX's platform.h
        #if    defined(__linux__)          \
            || defined(__FreeBSD__)        \
            || defined(__FreeBSD_kernel__) \
            || defined(__NetBSD__)         \
            || defined(__OpenBSD__)        \
            || defined(__DragonFly__)
            if (isWayland) {
            #ifdef PENUMBRA_WL_COMPAT // We check because wl_surface wouldn't exist if we don't have this macro
                // IMPLEMENT
            #endif
            } else {
            #ifdef PENUMBRA_X11_COMPAT
                // IMPLEMENT
            #endif
            }
        #elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
            // IMPLEMENT
        #elif defined(_WIN32) || defined(_WIN64)
            // IMPLEMENT
        #endif

        // Get VSync from PenumbraFlags
        if (penumbraFlags & PENUMBRA_VSYNC) {
            // Set VSync Here...
        }

        // Continue Initializing...

        // Enable debug text.
        if (penumbraFlags & PENUMBRA_DEBUG) {
            debugging = true;
        }

        // Set this object as Window User Pointer
        glfwSetWindowUserPointer(window, this);

        // Set time stuff
        currentTime = glfwGetTime();
        lastTime = currentTime;
    }

    // Deinitializes Backend
    void BackendWindow::deinitBackend() {
        // Delete WGPU-related stuff here...
    }

    // Creates a framebuffer and stores its ID (the Framebuffer is in GPU memory probably)
    void BackendWindow::createFramebuffers() {
        // Create 2D and 3D Framebuffers Here...
    }
}