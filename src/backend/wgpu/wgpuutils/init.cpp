#include "init.hpp"
#include "webgpu/webgpu.h"

#include <cassert>
#include <iostream>

#if __linux__
	// X11 Support
    #ifdef PENUMBRA_X11_COMPAT
		#define GLFW_EXPOSE_NATIVE_X11
	#endif
	// Wayland Support
	#ifdef PENUMBRA_WL_COMPAT
		#include <wayland-egl.h>
		#define GLFW_EXPOSE_NATIVE_WAYLAND
	#endif
#elif defined(_WIN32) || defined(_WIN64)
    #define GLFW_EXPOSE_NATIVE_WIN32
#elif defined(__ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__)
    #define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>

WGPUInstance createWGPUInstance() {
    #ifdef WGPU_EMSCRIPTEN
        // return instance
        return wgpuCreateInstance(nullptr);
    #else
        // Create Descriptor
        WGPUInstanceDescriptor descriptor = {};
        descriptor.nextInChain = nullptr;

        // return instance
        return wgpuCreateInstance(&descriptor);
    #endif
}

// FUNCTION GRABBED FROM https://github.com/eliemichel/LearnWebGPU-Code/blob/step005/main.cpp
/**
* Utility function to get a WebGPU adapter, so that
*     WGPUAdapter adapter = requestAdapterSync(options);
* is roughly equivalent to
*     const adapter = await navigator.gpu.requestAdapter(options);
*/
WGPUAdapter requestAdapterSync(WGPUInstance instance, WGPURequestAdapterOptions const * options) {
    // A simple structure holding the local information shared with the
    // onAdapterRequestEnded callback.
    struct UserData {
        WGPUAdapter adapter = nullptr;
        bool requestEnded = false;
    };
    UserData userData;

    // Callback called by wgpuInstanceRequestAdapter when the request returns
    // This is a C++ lambda function, but could be any function defined in the
    // global scope. It must be non-capturing (the brackets [] are empty) so
    // that it behaves like a regular C function pointer, which is what
    // wgpuInstanceRequestAdapter expects (WebGPU being a C API). The workaround
    // is to convey what we want to capture through the pUserData pointer,
    // provided as the last argument of wgpuInstanceRequestAdapter and received
    // by the callback as its last argument.
    auto onAdapterRequestEnded = [](WGPURequestAdapterStatus status, WGPUAdapter adapter, char const * message, void * pUserData) {
        UserData& userData = *reinterpret_cast<UserData*>(pUserData);
        if (status == WGPURequestAdapterStatus_Success) {
            userData.adapter = adapter;
        } else {
            std::cout << "PENUMBRA_ERROR [WGPU]: Could not get WebGPU adapter: " << message << std::endl;
        }
        userData.requestEnded = true;
    };

    // Call to the WebGPU request adapter procedure
    wgpuInstanceRequestAdapter(
        instance /* equivalent of navigator.gpu */,
        options,
        onAdapterRequestEnded,
        (void*)&userData
    );

    // We wait until userData.requestEnded gets true
    #ifdef __EMSCRIPTEN__
        while (!userData.requestEnded) {
            emscripten_sleep(100);
        }
    #endif // __EMSCRIPTEN__

    assert(userData.requestEnded);

    return userData.adapter;
}

void deviceLostCallback(WGPUDeviceLostReason reason, char const* message, void* /* pUserData */) {
    std::cout << "Device lost: reason " << reason;
    if (message) std::cout << " (" << message << ")";
    std::cout << std::endl;
};
WGPUDeviceDescriptor getDeviceDescriptor() {
    WGPUDeviceDescriptor deviceDesc = {};
    deviceDesc.nextInChain = nullptr;
    deviceDesc.label = "Default Device"; // anything works here, that's your call
    deviceDesc.requiredFeatureCount = 0; // we do not require any specific feature
    deviceDesc.requiredLimits = nullptr; // we do not require any specific limit
    deviceDesc.defaultQueue.nextInChain = nullptr;
    deviceDesc.defaultQueue.label = "default queue";
    // A function that is invoked whenever the device stops being available.
    deviceDesc.deviceLostCallback = deviceLostCallback;

    return deviceDesc;
}
// CODE GRABBED FROM https://eliemichel.github.io/LearnWebGPU/getting-started/adapter-and-device/the-device.html
/**
 * Utility function to get a WebGPU device, so that
 *     WGPUAdapter device = requestDeviceSync(adapter, options);
 * is roughly equivalent to
 *     const device = await adapter.requestDevice(descriptor);
 * It is very similar to requestAdapter
 */
WGPUDevice requestDeviceSync(WGPUAdapter adapter, WGPUDeviceDescriptor const * descriptor) {
    struct UserData {
        WGPUDevice device = nullptr;
        bool requestEnded = false;
    };
    UserData userData;

    auto onDeviceRequestEnded = [](WGPURequestDeviceStatus status, WGPUDevice device, char const * message, void * pUserData) {
        UserData& userData = *reinterpret_cast<UserData*>(pUserData);
        if (status == WGPURequestDeviceStatus_Success) {
            userData.device = device;
        } else {
            std::cout << "PENUMBRA_ERROR [WGPU]: Could not get WebGPU device: " << message << std::endl;
        }
        userData.requestEnded = true;
    };

    wgpuAdapterRequestDevice(
        adapter,
        descriptor,
        onDeviceRequestEnded,
        (void*)&userData
    );

    #ifdef __EMSCRIPTEN__
        while (!userData.requestEnded) {
            emscripten_sleep(100);
        }
    #endif // __EMSCRIPTEN__

    assert(userData.requestEnded);

    return userData.device;
}

void showAdapterProperties(WGPUAdapter adapter) {
    WGPUAdapterInfo info = {};
    info.nextInChain = nullptr;
    wgpuAdapterGetInfo(adapter, &info);
    std::cout << "Adapter info:" << std::endl;
    std::cout << " - vendorID: " << info.vendorID << std::endl;
    if (info.vendor) {
        std::cout << " - vendor: " << info.vendor << std::endl;
    }
    if (info.architecture) {
        std::cout << " - architecture: " << info.architecture << std::endl;
    }
    std::cout << " - deviceID: " << info.deviceID << std::endl;
    if (info.device) {
        std::cout << " - device: " << info.device << std::endl;
    }
    if (info.description) {
        std::cout << " - description: " << info.description << std::endl;
    }
    std::cout << std::hex;
    std::cout << " - adapterType: 0x" << info.adapterType << std::endl;

    switch (info.backendType) {
    case WGPUBackendType_Undefined:
        std::cout << " - backendType: Undefined" << std::endl;
        break;
    case WGPUBackendType_Null:
        std::cout << " - backendType: Null" << std::endl;
        break;
    case WGPUBackendType_WebGPU:
        std::cout << " - backendType: WebGPU" << std::endl;
        break;
    case WGPUBackendType_D3D11:
        std::cout << " - backendType: DirectX 11" << std::endl;
        break;
    case WGPUBackendType_D3D12:
        std::cout << " - backendType: DirectX 12" << std::endl;
        break;
    case WGPUBackendType_Metal:
        std::cout << " - backendType: Metal" << std::endl;
        break;
    case WGPUBackendType_Vulkan:
        std::cout << " - backendType: Vulkan" << std::endl;
        break;
    case WGPUBackendType_OpenGL:
        std::cout << " - backendType: OpenGL" << std::endl;
        break;
    case WGPUBackendType_OpenGLES:
        std::cout << " - backendType: OpenGL ES" << std::endl;
        break;
    case WGPUBackendType_Force32:
        std::cout << " - backendType: Force32 (what's that?)" << std::endl;
        break;
    }

    std::cout << std::dec; // Restore decimal numbers
}

// WINDOW
WGPUSurface getSurfaceFromGLFWWindow(GLFWwindow* window, WGPUInstance instance) {
    WGPUSurface surface;

    // Code adapted from WGPU-Native Example:
    // https://github.com/gfx-rs/wgpu-native/blob/trunk/examples/triangle/main.c
    // TODO: Adapt for Windows and MacOS
    #if defined(GLFW_EXPOSE_NATIVE_COCOA)
    {
        id metal_layer = NULL;
        NSWindow *ns_window = glfwGetCocoaWindow(window);
        [ns_window.contentView setWantsLayer:YES];
        metal_layer = [CAMetalLayer layer];
        [ns_window.contentView setLayer:metal_layer];
        surface = wgpuInstanceCreateSurface(
            instance,
            &(const WGPUSurfaceDescriptor){
                .nextInChain =
                    (const WGPUChainedStruct *)&(
                        const WGPUSurfaceDescriptorFromMetalLayer){
                        .chain =
                            (const WGPUChainedStruct){
                                .sType = WGPUSType_SurfaceDescriptorFromMetalLayer,
                            },
                        .layer = metal_layer,
                    },
            });
    }
    #elif defined(GLFW_EXPOSE_NATIVE_WAYLAND) && defined(GLFW_EXPOSE_NATIVE_X11)
    if (glfwGetPlatform() == GLFW_PLATFORM_X11) {
        Display *x11_display = glfwGetX11Display();
        Window x11_window = glfwGetX11Window(window);

        // Create Descriptor
        const WGPUChainedStruct chainedStruct {
            .next = nullptr,
            .sType = WGPUSType_SurfaceDescriptorFromXlibWindow,
        };
        const WGPUSurfaceDescriptorFromXlibWindow surfaceDesc {
            .chain = chainedStruct,
            .display = x11_display,
            .window = x11_window,
        };
        const WGPUSurfaceDescriptor desc {
            .nextInChain = (const WGPUChainedStruct*)&surfaceDesc,
            .label = "X11 Surface"
        };

        // Create Surface
        surface = wgpuInstanceCreateSurface(
            instance,
            &desc
        );
    }
    if (glfwGetPlatform() == GLFW_PLATFORM_WAYLAND) {
        struct wl_display *wayland_display = glfwGetWaylandDisplay();
        struct wl_surface *wayland_surface = glfwGetWaylandWindow(window);

        // Create Descriptor
        const WGPUChainedStruct chainedStruct {
            .next = nullptr,
            .sType = WGPUSType_SurfaceDescriptorFromWaylandSurface
        };
        const WGPUSurfaceDescriptorFromWaylandSurface surfaceDesc {
            .chain = chainedStruct,
            .display = wayland_display,
            .surface = wayland_surface,
        };
        const WGPUSurfaceDescriptor desc {
            .nextInChain = (const WGPUChainedStruct*)&surfaceDesc,
            .label = "Wayland Surface"
        };

        // Create Surface
        surface = wgpuInstanceCreateSurface(
            instance,
            &desc
        );
    }
    #elif defined(GLFW_EXPOSE_NATIVE_WIN32)
    {
        HWND hwnd = glfwGetWin32Window(window);
        HINSTANCE hinstance = GetModuleHandle(NULL);
        surface = wgpuInstanceCreateSurface(
            instance,
            &(const WGPUSurfaceDescriptor){
                .nextInChain =
                    (const WGPUChainedStruct *)&(
                        const WGPUSurfaceDescriptorFromWindowsHWND){
                        .chain =
                            (const WGPUChainedStruct){
                                .sType = WGPUSType_SurfaceDescriptorFromWindowsHWND,
                            },
                        .hinstance = hinstance,
                        .hwnd = hwnd,
                    },
            });
    }
    #else
    #error "Unsupported GLFW native platform"
    #endif

    return surface;
}