#include "../backendWindow.hpp"

#include "debug/log.hpp"

#include <GLFW/glfw3.h>

#include <X11/Xlib.h>
#include <bgfx/defines.h>
#include <bx/bx.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

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

#include <stdexcept>

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

    // BGFX Global Variables
    // ---------------------

    // Reset Flags (things like VSync or Antialiasing)
    uint32_t _resetFlags;


    // PUBLIC:

    // Shows the framebuffers used (2D and/or 3D) to the screen
    // and polls events
    void BackendWindow::update() {
		// TODO: Only set when changing state
        if (debugging) {
            // Set Debug mode to BGFX_DEBUG_WIREFRAME or BGFX_DEBUG_TEXT
            bgfx::setDebug(BGFX_DEBUG_TEXT);
            // Update Debug Text if we're debugging
            // TODO: Make console debug backend-independent
		    pen::debug::updateConsole();
        }
        else {
            bgfx::setDebug(BGFX_DEBUG_NONE);
        }

		// Advance to next frame. Process submitted rendering primitives.
		bgfx::frame();

        currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
    }

    // Set Window Size.
    // Rebuilds framebuffers
    void BackendWindow::resize(uint16_t x, uint16_t y) {
        width = x;
        height = y;

        bgfx::reset((uint32_t)width, (uint32_t)height, _resetFlags);
        createFramebuffers();
    }

    // PRIVATE:

    void BackendWindow::initBackendSpecific() {
        // Set GLFW window resize callback
        glfwSetFramebufferSizeCallback(window, callbacks::glfw_framebufferResizeCallback);
        // TODO: Set GLFW Input Callbacks
        // Set GLFW Input callbacks

        // Call bgfx::renderFrame before bgfx::init to signal to bgfx not to create a render thread.
        // Most graphics APIs must be used on the same thread that created the window.
        bgfx::renderFrame();

        // Initialize bgfx using the native window handle and window resolution.
        bgfx::Init init;

        // We can use BX stuff because we're in the BGFX backend and nobody can stop us >:D
        #if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
            if (isWayland) {
            #ifdef PENUMBRA_WL_COMPAT // We check because wl_surface wouldn't exist if we don't have this macro
                init.platformData.ndt = glfwGetWaylandDisplay();
                init.platformData.nwh = (void*)(struct wl_surface*)glfwGetWaylandWindow(window);
                init.platformData.type = bgfx::NativeWindowHandleType::Wayland;
            #endif
            } else {
            #ifdef PENUMBRA_X11_COMPAT
                init.platformData.ndt = glfwGetX11Display();
                init.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(window);
                init.platformData.type = bgfx::NativeWindowHandleType::Default;
            #endif
            }
        #elif BX_PLATFORM_OSX
            init.platformData.nwh = glfwGetCocoaWindow(window);
            init.platformData.type = bgfx::NativeWindowHandleType::Default;
        #elif BX_PLATFORM_WINDOWS
            init.platformData.nwh = glfwGetWin32Window(window);
            init.platformData.type = bgfx::NativeWindowHandleType::Default;
        #endif

        // GET RESET FLAGS
        if (penumbraFlags & PENUMBRA_VSYNC) {
            _resetFlags = _resetFlags | BGFX_RESET_VSYNC;
        }

        // INFO: Cursed cast
        glfwGetWindowSize(window, (int*)&width, (int*)&height);
        init.resolution.width = (uint32_t)width;
        init.resolution.height = (uint32_t)height;
        init.resolution.reset = _resetFlags;

        // Choose Vulkan backend
        init.type = bgfx::RendererType::Vulkan;
        
        // Init BGFX
        if (!bgfx::init(init)) {
            throw std::runtime_error("PENUMBRA_ERROR [BGFX]: Error while initializing BGFX");
        }

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

    // Creates a framebuffer and stores its ID (the Framebuffer is in GPU memory probably)
    void BackendWindow::createFramebuffers() {
        // Set view 0 to the same dimensions as the window and to clear the color buffer.
        bgfx::setViewClear(framebuffer3D, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, (penumbraFlags & PENUMBRA_TRANSPARENT)?0x00000000:0x7e7189ff, 1.0f, 0);
        bgfx::setViewRect(framebuffer3D, 0, 0, bgfx::BackbufferRatio::Equal);

        bgfx::setViewClear(framebuffer2D, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, (penumbraFlags & PENUMBRA_TRANSPARENT)?0x00000000:0x1e093600, 1.0f, 0);
        bgfx::setViewRect(framebuffer2D, 0, 0, bgfx::BackbufferRatio::Equal);
    }
}