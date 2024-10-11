#include "shader.hpp"

#include <bgfx/bgfx.h>
#include <bgfx/defines.h>
#include <bgfx/platform.h>
#include <bx/math.h>
#include <cstdint>
#include <stdexcept>
#include <string>

#include <GLFW/glfw3.h>
#if BX_PLATFORM_LINUX
	// X11 Support
    #ifdef PENUMBRA_X11_COMPAT
		#define GLFW_EXPOSE_NATIVE_X11
	#endif
	// Wayland Support
	#ifdef PENUMBRA_WL_COMPAT
		#include <wayland-egl.h>
		#define GLFW_EXPOSE_NATIVE_WAYLAND
	#endif
#elif BX_PLATFORM_WINDOWS
    #define GLFW_EXPOSE_NATIVE_WIN32
#elif BX_PLATFORM_OSX
    #define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>

int _width = 1280;
int _height = 720;
GLFWwindow* window;

struct PosUvVertex {
    float x;
    float y;
    float z;

    float u;
    float v;

    static bgfx::VertexLayout getVertexLayout() {
        // Init vertex attributes
        bgfx::VertexLayout vertexLayout;
        vertexLayout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();
        return vertexLayout;
    }
};

// Same for every object. Everything is based on squares in 2D
const PosUvVertex QUAD_VTX[] = {
    {0.5f, 0.5f, 0.0f,  1.0f, 1.0f},	// top right
    {0.5f, -.5f, 0.0f,  1.0f, 0.0f},	// bottom right
    {-.5f, -.5f, 0.0f,  0.0f, 0.0f},	// bottom left
    {-.5f, 0.5f, 0.0f,  0.0f, 1.0f} 	// top left
};
const uint16_t QUAD_IDX[] = {
    0, 1, 3,
    1, 2, 3
};

static void glfw_errorCallback(int error, const char *description)
{
    fprintf(stderr, "GLFW error %d: %s\n", error, description);
}
static void glfw_framebufferResizeCallback(GLFWwindow* window, int width, int height) {
    _width = width;
    _height = height;

    bgfx::reset((uint32_t)width, (uint32_t)height, 0);
    bgfx::setViewRect(0, 0, 0, bgfx::BackbufferRatio::Equal);
}

void createWindow() {
    glfwSetErrorCallback(glfw_errorCallback);

    // Linux-specific code related to window creation
	#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
		// Create a variable indicating if we're using wayland or not.
		// By default we'll say that we're on X11
		bool isWayland = false;
		#ifdef PENUMBRA_X11_COMPAT
			glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
		#elif defined (PENUMBRA_WL_COMPAT)
			glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
			isWayland = true; // Tell the program that we're using wayland, not X11
		#else
			throw std::runtime_error("PENUMBRA_ERROR: Tried to create X11 and Wayland windows but neither are supported. Rebuild library with X11 and/or Wayland support");
		#endif
	#endif

		if (!glfwInit()) {
			throw std::runtime_error("PENUMBRA: Couldn't initialize GLFW");
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		_width = _width;
		_height = _height;
		window = glfwCreateWindow(_width, _height, "Penumbra", NULL, NULL);
		if (!window) {
			throw std::runtime_error("PENUMBRA: Error during window creation");
		}

		// Set callbacks to window
		glfwSetFramebufferSizeCallback(window, glfw_framebufferResizeCallback);

		// Call bgfx::renderFrame before bgfx::init to signal to bgfx not to create a render thread.
		// Most graphics APIs must be used on the same thread that created the window.
		bgfx::renderFrame();

		// Initialize bgfx using the native window handle and window resolution.
		bgfx::Init init;
	#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
		if (isWayland) {
		#ifdef PENUMBRA_WL_COMPAT // We check because wl_surface wouldn't exist if we don't have this macro
			init.platformData.ndt = glfwGetWaylandDisplay();
			init.platformData.nwh = (void*)(struct wl_surface*)glfwGetWaylandWindow(window);
			init.platformData.type = bgfx::NativeWindowHandleType::Wayland;
		#endif
		} else {
		#ifdef PENUMBRA_X11_COMPAT
			init.platformData.ndt = glfwGetX11Display();
			init.platformData.nwh = (void*)(uintptr_t)glfwGetX11Window(window);
			init.platformData.type = bgfx::NativeWindowHandleType::Default;
		#endif
		}
	#elif BX_PLATFORM_OSX
		init.platformData.nwh = glfwGetCocoaWindow(window);
		init.platformData.type = bgfx::NativeWindowHandleType::Default;
	#elif BX_PLATFORM_WINDOWS
		init.platformData.nwh = glfwGetWin32Window(window);
		init.platformData.type = bgfx::NativeWindowHandleType::Default;
	#endif

		glfwGetWindowSize(window, &_width, &_height);
		init.resolution.width = (uint32_t)_width;
		init.resolution.height = (uint32_t)_height;
		init.resolution.reset = 0;

		// Choose Vulkan backend
		init.type = bgfx::RendererType::Vulkan;

		if (!bgfx::init(init)) {
			throw std::runtime_error("PENUMBRA: Error while initializing BGFX");
		}
        
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x1e093600, 1.0f, 0);
		bgfx::setViewRect(0, 0, 0, bgfx::BackbufferRatio::Equal);

		// Enable debug text.
		bgfx::setDebug(BGFX_DEBUG_TEXT);
}

int main() {
    createWindow();

    pen::antumbra::Shader s(0, "pnmbr/shaders-2D/default2");

    const bgfx::VertexLayout vtxLayout = PosUvVertex::getVertexLayout();
        
    bgfx::VertexBufferHandle vbh = bgfx::createVertexBuffer(bgfx::makeRef(QUAD_VTX, sizeof(QUAD_VTX)), vtxLayout);
    bgfx::IndexBufferHandle ibh = bgfx::createIndexBuffer(bgfx::makeRef(QUAD_IDX, sizeof(QUAD_IDX)));

    // Run Program
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        // This dummy draw call is here to make sure that view 0 is cleared if no other draw calls are submitted to view 0.
		bgfx::touch(0);

		const bx::Vec3 at  = { 0.0f, 0.0f, 0.0f };
        const bx::Vec3 eye = { 0.0f, 0.0f, 2.0f };

        // Set view and projection matrix for view 0.
        float viewMtx[16];
        bx::mtxLookAt(viewMtx, eye, at);

        float projMtx[16];
        bx::mtxProj(projMtx,
                    60.0f,
                    float(_width)/float(_height),
                    0.1f, 100.0f,
                    bgfx::getCaps()->homogeneousDepth);

        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x1e093600, 1.0f, 0);

        bgfx::setViewTransform(0, viewMtx, projMtx);

        bgfx::setVertexBuffer(0, vbh);
        bgfx::setIndexBuffer(ibh);

        bgfx::setState(BGFX_STATE_DEFAULT);
        bgfx::submit(0, s.getProgram());
        
		// Advance to next frame. Process submitted rendering primitives.
		bgfx::frame();
    }
}