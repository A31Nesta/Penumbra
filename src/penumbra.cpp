#include "penumbra.hpp"

// STD
#include <cstring>
#include <string>
#include <cstdint>

// CUSTOM
#include "core/windowCoordinator.hpp"

namespace pen {
	uint32_t win;

	void init(std::string title, int width, int height, uint32_t penumbra_flags) {
		uint32_t win = core::createWindow(title, width, height, penumbra_flags);
	}

	void update() {
		core::updateWindows();
	}

	// void end() {
	// 	bgfx::shutdown();
	// 	glfwTerminate();
	// }
}