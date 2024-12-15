#include <GLFW/glfw3.h>
#include <webgpu/webgpu.h>

WGPUInstance createWGPUInstance();

WGPUAdapter requestAdapterSync(WGPUInstance instance, WGPURequestAdapterOptions const* options);

void deviceLostCallback(WGPUDeviceLostReason reason, char const* message, void* /* pUserData */);
WGPUDeviceDescriptor getDeviceDescriptor(WGPUAdapter adapter, WGPURequiredLimits& limits);
WGPUDevice requestDeviceSync(WGPUAdapter adapter, WGPUDeviceDescriptor const* descriptor);

void showAdapterProperties(WGPUAdapter adapter);

// Limits
void setDefault(WGPULimits& limits);
WGPURequiredLimits getRequiredLimits(WGPUAdapter adapter);

// WINDOW
WGPUSurface getSurfaceFromGLFWWindow(GLFWwindow* window, WGPUInstance instance);