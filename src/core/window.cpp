#include "window.hpp"
#include "antumbra/antumbra.hpp"
#include "core/windowStruct.hpp"
#include "debug/log.hpp"
#include "utils/config.hpp"

#include <bgfx/defines.h>
#include <bx/bx.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <GLFW/glfw3.h>
#include <cmath>
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

#include <iostream>
#include <stdexcept>

namespace pen::core {
    namespace callbacks {
        static void glfw_errorCallback(int error, const char *description)
        {
            fprintf(stderr, "GLFW error %d: %s\n", error, description);
        }
        static void glfw_framebufferResizeCallback(GLFWwindow* window, int width, int height) {
            Window* win = (Window*)glfwGetWindowUserPointer(window);
            WindowStruct& winStruct = win->getWinStruct();

            winStruct._width = width;
		    winStruct._height = height;

            bgfx::reset((uint32_t)winStruct._width, (uint32_t)winStruct._height, winStruct._resetFlags);
            bgfx::setViewRect(winStruct.view3D, 0, 0, bgfx::BackbufferRatio::Equal);
            bgfx::setViewRect(winStruct.view2D, 0, 0, bgfx::BackbufferRatio::Equal);

			bgfx::setViewClear(winStruct.view3D, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, (winStruct._penumbraFlags & PENUMBRA_TRANSPARENT)?0x00000000:0x7e7189ff, 1.0f, 0);
			bgfx::setViewClear(winStruct.view2D, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, (winStruct._penumbraFlags & PENUMBRA_TRANSPARENT)?0x00000000:0x1e093600, 1.0f, 0);
        }
    }

    Window::Window(std::string title, int width, int height, uint32_t penumbra_flags, uint16_t view3D, uint16_t view2D) {
        winStruct.view3D = view3D;
        winStruct.view2D = view2D;
        init(title, width, height, penumbra_flags);
    }
	Window::~Window() {
		glfwDestroyWindow(winStruct._window);
	}
    
    // Set behavior properties
    void Window::setWindowCanClose(bool canClose) {
        this->canClose = canClose;
    }
    bool Window::windowCanClose() {
        return canClose;
    }

	// Create or set renderers
	void Window::createAntumbra(std::string defaultShader) {
		antumbra = new antumbra::Antumbra(defaultShader);
		debug::print("LOG: Antumbra renderer successfully created\n");
	}


    // Normal functions
    void Window::update() {
		currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;

		draw();
    }
    void Window::end() {
        glfwSetWindowShouldClose(winStruct._window, GLFW_TRUE);
    }
    bool Window::running() {
        return !glfwWindowShouldClose(winStruct._window);
    }

    WindowStruct& Window::getWinStruct() {
        return winStruct;
    }


    // PRIVATE
    void Window::init(std::string title, int width, int height, uint32_t penumbra_flags) {
        // Init GLFW
		glfwSetErrorCallback(callbacks::glfw_errorCallback);

		// Linux-specific code related to window creation
	#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
		// Create a variable indicating if we're using wayland or not.
		// By default we'll say that we're on X11
		bool isWayland = false;

		// If the wayland flag was passed try to create a wayland window
		if (penumbra_flags & PENUMBRA_WAYLAND) {
			// If Wayland is supported, create the window
			#ifdef PENUMBRA_WL_COMPAT
				glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
				isWayland = true; // Tell the program that we're using wayland, not X11
			#else
				throw std::runtime_error("PENUMBRA_ERROR: -wayland Argument passed but Wayland support is disabled");
			#endif
		} else {
			// If the flag was not passed, try to create an X11 window.
			// If that's not possible, create a Wayland window
			// If that's not possible, implode
			#ifdef PENUMBRA_X11_COMPAT
				glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
			#elif defined (PENUMBRA_WL_COMPAT)
				glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
				isWayland = true; // Tell the program that we're using wayland, not X11
			#else
				throw std::runtime_error("PENUMBRA_ERROR: Tried to create X11 and Wayland windows but neither are supported. Rebuild library with X11 and/or Wayland support");
			#endif
		}
	#endif

		if (!glfwInit()) {
			throw std::runtime_error("PENUMBRA: Couldn't initialize GLFW");
		}

		if (penumbra_flags & PENUMBRA_TRANSPARENT) {
			glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
			std::cout << "PENUMBRA_WARNING: Transparency Enabled. Bugs WILL happen\n";
		} else {
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		}

		winStruct._penumbraFlags = penumbra_flags;
		winStruct._width = width;
		winStruct._height = height;
		winStruct._window = glfwCreateWindow(width, height, "Penumbra", NULL, NULL);
		if (!winStruct._window) {
			throw std::runtime_error("PENUMBRA: Error during window creation");
		}

		// Set callbacks to window
		glfwSetFramebufferSizeCallback(winStruct._window, callbacks::glfw_framebufferResizeCallback);

		// Call bgfx::renderFrame before bgfx::init to signal to bgfx not to create a render thread.
		// Most graphics APIs must be used on the same thread that created the window.
		bgfx::renderFrame();

		// Initialize bgfx using the native window handle and window resolution.
		bgfx::Init init;
	#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
		if (isWayland) {
		#ifdef PENUMBRA_WL_COMPAT // We check because wl_surface wouldn't exist if we don't have this macro
			init.platformData.ndt = glfwGetWaylandDisplay();
			init.platformData.nwh = (void*)(struct wl_surface*)glfwGetWaylandWindow(winStruct._window);
			init.platformData.type = bgfx::NativeWindowHandleType::Wayland;
		#endif
		} else {
		#ifdef PENUMBRA_X11_COMPAT
			init.platformData.ndt = glfwGetX11Display();
			init.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(winStruct._window);
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
		if (penumbra_flags & PENUMBRA_VSYNC) {
			winStruct._resetFlags = winStruct._resetFlags | BGFX_RESET_VSYNC;
		}

		glfwGetWindowSize(winStruct._window, &width, &height);
		init.resolution.width = (uint32_t)width;
		init.resolution.height = (uint32_t)height;
		init.resolution.reset = winStruct._resetFlags;

		// Choose Vulkan backend
		init.type = bgfx::RendererType::Vulkan;

		if (!bgfx::init(init)) {
			throw std::runtime_error("PENUMBRA: Error while initializing BGFX");
		}

		// Set view 0 to the same dimensions as the window and to clear the color buffer.
		bgfx::setViewClear(winStruct.view3D, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, (penumbra_flags & PENUMBRA_TRANSPARENT)?0x00000000:0x7e7189ff, 1.0f, 0);
		bgfx::setViewRect(winStruct.view3D, 0, 0, bgfx::BackbufferRatio::Equal);

		// INFO: View Clear Disabled for 2D. Test
        bgfx::setViewClear(winStruct.view2D, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, (winStruct._penumbraFlags & PENUMBRA_TRANSPARENT)?0x00000000:0x1e093600, 1.0f, 0);
		bgfx::setViewRect(winStruct.view2D, 0, 0, bgfx::BackbufferRatio::Equal);

		// Enable debug text.
		if (penumbra_flags & PENUMBRA_DEBUG) {
			winStruct.debugging = true;
		}

        // SET USER POINTER
        glfwSetWindowUserPointer(winStruct._window, this);
		// SET TIME STUFF
		currentTime = glfwGetTime();
		lastTime = currentTime;
    }

    void Window::draw() {
        // This dummy draw call is here to make sure that view 0 is cleared if no other draw calls are submitted to view 0.
		bgfx::touch(winStruct.view3D);

		// If we have a 2D renderer
		if (antumbra != nullptr) {
			antumbra->draw(winStruct.view2D, winStruct._width, winStruct._height);
		} else {
			// bgfx::touch(winStruct.view2D);
		}

		// Debug Text
		// TODO: Only set when changing state
		if (winStruct.debugging) {
			bgfx::setDebug(BGFX_DEBUG_WIREFRAME);
		} else {
			bgfx::setDebug(BGFX_DEBUG_NONE);
		}

		// Update Debug Text
		pen::debug::updateConsole();
		// Advance to next frame. Process submitted rendering primitives.
		bgfx::frame();
    }
}