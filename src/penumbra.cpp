#include <bx/bx.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>

#include <GLFW/glfw3.h>
#if BX_PLATFORM_LINUX
    #define GLFW_EXPOSE_NATIVE_X11
#elif BX_PLATFORM_WINDOWS
    #define GLFW_EXPOSE_NATIVE_WIN32
#elif BX_PLATFORM_OSX
    #define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>

// STD
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <string>
#include <cstdint>

// CUSTOM
#include "utils/config.hpp"

static void glfw_errorCallback(int error, const char *description)
{
	fprintf(stderr, "GLFW error %d: %s\n", error, description);
}

int main(int argc, char** argv) {
	uint32_t penumbra_flags = pen::getFlagsFromArguments(argc, argv);

    // Init GLFW
    glfwSetErrorCallback(glfw_errorCallback);

#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    if (penumbra_flags & PENUMBRA_WAYLAND) {
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
    } else {
        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
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

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Penumbra", NULL, NULL);
    if (!window) {
        throw std::runtime_error("PENUMBRA: Error during window creation");
    }

    // Call bgfx::renderFrame before bgfx::init to signal to bgfx not to create a render thread.
	// Most graphics APIs must be used on the same thread that created the window.
	bgfx::renderFrame();

    // Initialize bgfx using the native window handle and window resolution.
	bgfx::Init init;
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
    // No native wayland support
    // TODO: Native Wayland
	init.platformData.ndt = glfwGetX11Display();
	init.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(window);
#elif BX_PLATFORM_OSX
	init.platformData.nwh = glfwGetCocoaWindow(window);
#elif BX_PLATFORM_WINDOWS
	init.platformData.nwh = glfwGetWin32Window(window);
#endif

    int width, height;
	glfwGetWindowSize(window, &width, &height);
    std::cout << "WINDOW SIZE " << std::to_string(width) << ", " << std::to_string(height) << "\n";
	init.resolution.width = (uint32_t)width;
	init.resolution.height = (uint32_t)height;
	init.resolution.reset = BGFX_RESET_VSYNC;

    // Choose Vulkan backend
    // TODO: Also support OpenGL if Vulkan is not supported
    init.type = bgfx::RendererType::Vulkan;

    if (!bgfx::init(init)) {
        throw std::runtime_error("PENUMBRA: Error while initializing BGFX");
    }

	// Set view 0 to the same dimensions as the window and to clear the color buffer.
	const bgfx::ViewId kClearView = 0;
	bgfx::setViewClear(kClearView, BGFX_CLEAR_COLOR, 0x00000000);
	bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);

    // Enable debug text.
    bgfx::setDebug(BGFX_DEBUG_TEXT);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		// Handle window resize.
		int oldWidth = width, oldHeight = height;
		glfwGetWindowSize(window, &width, &height);
		if (width != oldWidth || height != oldHeight) {
			bgfx::reset((uint32_t)width, (uint32_t)height, BGFX_RESET_VSYNC);
			bgfx::setViewRect(kClearView, 0, 0, bgfx::BackbufferRatio::Equal);
		}
		// This dummy draw call is here to make sure that view 0 is cleared if no other draw calls are submitted to view 0.
		bgfx::touch(kClearView);
		// Use debug font to print information about this example.
		bgfx::dbgTextClear();
		bgfx::dbgTextPrintf(1, 1, 0x0f, "PENUMBRA: Test Version");
		bgfx::dbgTextPrintf(80, 1, 0x0f, "\x1b[;0m    \x1b[;1m    \x1b[; 2m    \x1b[; 3m    \x1b[; 4m    \x1b[; 5m    \x1b[; 6m    \x1b[; 7m    \x1b[0m");
		bgfx::dbgTextPrintf(80, 2, 0x0f, "\x1b[;8m    \x1b[;9m    \x1b[;10m    \x1b[;11m    \x1b[;12m    \x1b[;13m    \x1b[;14m    \x1b[;15m    \x1b[0m");
		const bgfx::Stats* stats = bgfx::getStats();
		bgfx::dbgTextPrintf(1, 2, 0x0f, "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in characters.", stats->width, stats->height, stats->textWidth, stats->textHeight);

		// EXTRA DEBUGGING INFO
		if (penumbra_flags & PENUMBRA_TRANSPARENT) {
			bgfx::dbgTextPrintf(0, stats->textHeight-1, 0xcf, " TRANSPARENCY ENABLED: PREPARE FOR BUGS ");
		}
		if (penumbra_flags & PENUMBRA_WAYLAND) {
			bgfx::dbgTextPrintf(stats->textWidth-42, stats->textHeight-1, 0xe8, " WARNING: Wayland currently not supported ");
		}
		
		// Advance to next frame. Process submitted rendering primitives.
		bgfx::frame();
	}
	bgfx::shutdown();
	glfwTerminate();
	return 0;
}
