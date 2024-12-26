# Penumbra

Simple Renderer built for Selengine, a custom 3D game engine.   
It features a strange and simplistic lighting model based on textures created specifically for the game being developed in Selengine.   
Cross-platform but primarily tested and developed on Linux.

It supports **BGFX** and **WebGPU** as backends (WebGPU via WGPU-Native).

## Important

This is a Work In Progress. As of now, only 2D Rendering is supported.  
Furthermore, the BGFX backend is more developed than the WebGPU one:
The BGFX backend can output debug information to the screen while the
WGPU one outputs to the console, for example.

## What does it support?

- 2D and 3D Graphics via separate modules (Antumbra and Umbra respectively)
- Simple phong-based lighting
- Model loading

### What does it ACTUALLY support?

Right now it only supports simple 2D graphics, but they work in exactly the same way with WebGPU and BGFX.

## "Is there a lore reason why it's called like that?"

Yes, there is.  
There is a lore reason related to the main projects (Selengine and the game being made with it)

## Dependencies
### Utilities
- **bgfx-shaderc** (included with BGFX) \[Only if using BGFX as backend\]
- **toktx** \[Khronos Textures not supported with WebGPU Backend for now\]

### Libraries
- **BGFX** \[Only if using BGFX as backend\]
- **WGPU-Native**: Version 22.1.0.5 \[Only if using WebGPU as backend\]
- **GLFW**
- **GLM**
- **STB Image** (included)

### Extra: Arch Linux Packages

If you're using Arch Linux or an Arch-based Linux distribution, you can
use these packages to install the necessary dependencies.
WGPU-Native is obtained from the Releases tab of [its repository](https://github.com/gfx-rs/wgpu-native/).

- **bgfx-cmake-git** \[Only if using BGFX as backend\]
- **ktx_software-git** \[Only if using BGFX as backend; KTX is currently not supported with WGPU Backend\]
- **glfw**
- **glm**