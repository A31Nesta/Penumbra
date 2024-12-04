#include "window.hpp"

#include "antumbra/antumbra.hpp"
#include "backend/backendWindow.hpp"
#include "debug/log.hpp"

#include <cmath>

namespace pen::core {
    Window::Window(std::string title, int width, int height, uint32_t penumbra_flags)
	: window(title, width, height, penumbra_flags) {
        
    }
	Window::~Window() {
		// Delete Antumbra if it exists
		if (antumbra != nullptr) {
			delete antumbra;
		}
	}
    
    // Tells the window to close after this update is done
	void Window::closeWindow() {
		window.closeWindow();
	}
	// Returns false if window should close
	bool Window::running() {
		return window.running();
	}

	// Create or set renderers
	void Window::createAntumbra(std::string defaultShader) {
		antumbra = new antumbra::Antumbra(defaultShader);
		debug::print("PENUMBRA_LOG [BGFX]: Antumbra renderer successfully created\n");
	}

    // Normal functions
    void Window::update() {
		draw();

		// Update is done at the end
		window.update();
    }

	// Gets delta time from window
	double Window::getDeltaTime() {
		return window.getDeltaTime();
	}

    // PRIVATE
    void Window::draw() {
		// If we have a 2D renderer
		if (antumbra != nullptr) {
			antumbra->draw(window.get2DFramebuffer(), window.getWidth(), window.getHeight());
		}
    }
}