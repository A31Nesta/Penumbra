#include "windowCoordinator.hpp"
#include "debug/log.hpp"
#include "window.hpp"
#include <GLFW/glfw3.h>
#include <cstdint>
#include <string>
#include <vector>

namespace pen::core {
    namespace {
        std::vector<Window*> windows;
    }

    uint32_t createWindow(std::string title, int width, int height, uint32_t flags) {
        uint32_t id = windows.size();
        
        Window* w = new Window(title, width, height, flags, id*2, id*2+1);
        windows.push_back(w);

        return id;
    }
    void updateWindows() {
        uint32_t size = windows.size();

        glfwPollEvents();

        for (uint32_t i = 0; i < size; i++) {
            Window* w = windows.at(i);
            if (w == nullptr)
                continue;

            if (w->running()) {
                w->update();
            }
            // Only delete if we allow it to be deleted lmao
            else if (w->windowCanClose()) {
                delete w;
                windows.at(i) = nullptr;
            }
            else {
                debug::print("ERROR: Can't close this window.\n> Reason: Window "+std::to_string(i)+" has canClose set to false.\n> You can force-close all with closeAll()");
            }
        }
    }
    void closeWindow(uint32_t window) {
        windows.at(window)->end();
    }
    void closeAll() {

    }
}