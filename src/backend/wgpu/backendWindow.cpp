#include "../backendWindow.hpp"
#include "wgpu/wgpu.h"
#include "webgpu/webgpu.h"

#include <GLFW/glfw3.h>

#include <cassert>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>

// WGPU
#include "wgpuutils/init.hpp"

#ifdef __EMSCRIPTEN__
    #include <emscripten.h>
#endif // __EMSCRIPTEN__


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

    // WGPU Global Variables
    // ---------------------
    WGPUDevice device;
    WGPUQueue queue;
    WGPUSurface surface;


    // PUBLIC:

    // Shows the framebuffers used (2D and/or 3D) to the screen
    // and polls events
    void BackendWindow::update() {
        glfwPollEvents();

		wgpuDevicePoll(device, false, nullptr);

        currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
    }

    // Set Window Size.
    // Rebuilds framebuffers
    void BackendWindow::resize(uint16_t x, uint16_t y) {
        width = x;
        height = y;

        // config.width = width;
        // config.height = height;
        // wgpuSurfaceConfigure(surface, &config);
    }

    // PRIVATE:

    void BackendWindow::initBackendSpecific() {
        // Set GLFW window resize callback
        glfwSetFramebufferSizeCallback(window, callbacks::glfw_framebufferResizeCallback);
        // TODO: Set GLFW Input Callbacks
        // Set GLFW Input callbacks

        // Create WGPU Instance
        WGPUInstance instance = createWGPUInstance();
        if (!instance) {
            throw std::runtime_error("PENUMBRA_ERROR [WGPU]: Could not initialize WebGPU!");
        }

        // Get Window Surface
        surface = getSurfaceFromGLFWWindow(window, instance);

        // Request Adapter
        WGPURequestAdapterOptions adapterOpts = {};
        adapterOpts.nextInChain = nullptr;
        adapterOpts.compatibleSurface = surface;
        WGPUAdapter adapter = requestAdapterSync(instance, &adapterOpts);

        // We have the adapter, we can release the instance
        wgpuInstanceRelease(instance);
        std::cout << "PENUMBRA [WGPU]: Got adapter: " << adapter << std::endl;

        // Show adapter info (Useful to see what GPU is being used and what graphics API is being used)
        showAdapterProperties(adapter);

        // Create Descriptor
        WGPUDeviceDescriptor deviceDesc = getDeviceDescriptor();
        // Request device
        device = requestDeviceSync(adapter, &deviceDesc);

        // Release the adapter because we have a device
        wgpuAdapterRelease(adapter);

        // Get WebGPU Queue
        queue = wgpuDeviceGetQueue(device);
        

        // Linux and BSD (BSD was in a BGFX example when I started this project so...)
        // Code for these defines totally not yoinked from BX's platform.h
        #if    defined(__linux__)          \
            || defined(__FreeBSD__)        \
            || defined(__FreeBSD_kernel__) \
            || defined(__NetBSD__)         \
            || defined(__OpenBSD__)        \
            || defined(__DragonFly__)
            if (isWayland) {
            #ifdef PENUMBRA_WL_COMPAT // We check because wl_surface wouldn't exist if we don't have this macro
                // IMPLEMENT
            #endif
            } else {
            #ifdef PENUMBRA_X11_COMPAT
                // IMPLEMENT
            #endif
            }
        #elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
            // IMPLEMENT
        #elif defined(_WIN32) || defined(_WIN64)
            // IMPLEMENT
        #endif

        // Get VSync from PenumbraFlags
        if (penumbraFlags & PENUMBRA_VSYNC) {
            // Set VSync Here...
        }

        // Continue Initializing...

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

    // Deinitializes Backend
    void BackendWindow::deinitBackend() {
        wgpuQueueRelease(queue);
        wgpuSurfaceRelease(surface);
        wgpuDeviceRelease(device);
    }

    // Creates a framebuffer and stores its ID (the Framebuffer is in GPU memory probably)
    void BackendWindow::createFramebuffers() {
        // Create 2D and 3D Framebuffers Here...
    }
}