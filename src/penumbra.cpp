#include "penumbra.hpp"

// Libs
#include <GLFW/glfw3.h>
#include <bgfx/bgfx.h>

// STD
#include <cstring>
#include <string>
#include <cstdint>

// CUSTOM
#include "antumbra/antumbra.hpp"
#include "core/window.hpp"
#include "debug/log.hpp"

namespace pen {
	core::Window* win;
	antumbra::Antumbra* ant;

	core::Window* init(std::string title, int width, int height, uint32_t penumbra_flags) {
		win = new core::Window(title, width, height, penumbra_flags, 0, 1);
		win->createAntumbra("pnmbr/shaders-2D/default");

		ant = win->getAntumbra();
		return win;
	}

	void update() {
		if (!win->running()) {
			return;
		}

		glfwPollEvents();
		win->update();
	}

	void end() {
		debug::clearAll();
		bgfx::shutdown();
		glfwTerminate();
	}

	bool running() {
		return win->running();
	}

	core::Window* getWindow() {
		return win;
	}
	antumbra::Antumbra* getAntumbra() {
		return ant;
	}
}