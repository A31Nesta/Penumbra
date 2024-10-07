#pragma once

#include <GLFW/glfw3.h>
#include <bgfx/bgfx.h>

namespace pen::core {
    struct WindowStruct {
        bgfx::ViewId view3D;
        bgfx::ViewId view2D;
        GLFWwindow* _window;
        int _width;
        int _height;
        uint32_t _resetFlags = 0;
    };
}