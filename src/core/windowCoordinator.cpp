#include "windowCoordinator.hpp"
#include "debug/consoleColors.hpp"
#include "debug/log.hpp"
#include "window.hpp"

#include <GLFW/glfw3.h>

#include <cstdint>
#include <string>
#include <vector>

namespace pen::core {
    namespace {
        std::vector<Window*> windows;
        bool isRunning = true;
    }

    uint32_t createWindow(std::string title, int width, int height, uint32_t flags) {
        uint32_t id = windows.size();
        
        Window* w = new Window(title, width, height, flags, id*2, id*2+1);
        windows.push_back(w);

        return id;
    }
    void setWindowCloseable(uint32_t window, bool closeable) {
        Window* w = windows.at(window);
        if (w == nullptr)
            return;
        
        w->setWindowCanClose(closeable);
    }

    // Create or set renderers for windows
    void createAntumbraForWindow(uint32_t window, std::string defaultShader) {
        windows.at(window)->createAntumbra(defaultShader);
    }

    // Get Window
    Window* getWindow(uint32_t window) {
        return windows.at(window);
    }


    void updateWindows() {
        uint32_t size = windows.size();

        glfwPollEvents();

        isRunning = false;
        for (uint32_t i = 0; i < size; i++) {
            Window* w = windows.at(i);
            if (w == nullptr)
                continue;

            if (w->running()) {
                isRunning = true;
                w->update();
            }
            // Only delete if we allow it to be deleted lmao
            else if (w->windowCanClose()) {
                delete w;
                windows.at(i) = nullptr;
            }
            else {
                isRunning = true;
                glfwSetWindowShouldClose(w->getWinStruct()._window, GLFW_FALSE);
                debug::print(
                    "ERROR: Can't close this window.\n> Reason: Window "+std::to_string(i)+" has canClose set to false.\n> You can force-close all with closeAll()\n\n",
                    debug::Color::DARK_RED
                );
            }
        }
    }
    void closeWindow(uint32_t window) {
        windows.at(window)->end();
    }
    void closeAll() {
        // TODO: Add CloseAll
    }

    bool running() {
        return isRunning;
    }
}