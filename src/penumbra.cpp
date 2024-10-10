#include "penumbra.hpp"

// Libs
#include <GLFW/glfw3.h>
#include <bgfx/bgfx.h>

// STD
#include <cstring>
#include <string>
#include <cstdint>

// CUSTOM
#include "core/windowCoordinator.hpp"
#include "debug/log.hpp"

namespace pen {
	uint32_t win;

	void init(std::string title, int width, int height, uint32_t penumbra_flags) {
		uint32_t win = core::createWindow(title, width, height, penumbra_flags);
		core::createAntumbraForWindow(win, "pnmbr/shaders-2D/default2");
	}

	void update() {
		core::updateWindows();
	}

	void end() {
		debug::clearAll();
		bgfx::shutdown();
		glfwTerminate();
	}

	bool running() {
		return core::running();
	}
}