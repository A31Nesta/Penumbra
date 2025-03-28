<img src="readme-assets/penumbra-rotate.svg" alt="EcLibs Penumbra"></img>

This project has been cancelled. I enjoyed WGPU more than BGFX but I had too many issues with building WGPU-Native so
I just went to learn Rust and use normal WGPU instead. Turns out Rust has a good package manager and easy cross-compilation
so... Once the new version is at least semi-usable I'll link it here.

---

Penumbra ~~is~~ WAS a simple Rendering Engine primarily built for Occlusion, a custom 3D game engine.  
Cross-platform but mainly tested and developed on Linux.  
Penumbra is part of the EcLibs project, a group of libraries and tools mainly focused on graphics and game development.  
Like with **Occlusion**, the name of this project is also related to eclipses. Why? Rule of cool.

It supports **BGFX** and **WebGPU** as backends (WebGPU via WGPU-Native).

## Important

This is a Work in Progress. As of now, only 2D Rendering is supported.

## What does it support?

- 2D and 3D Graphics via separate modules (Antumbra and Umbra respectively)
- Phong-based lighting and other simple experimental non-physically based lighting models
- Model loading

### What does it ACTUALLY support?

Right now it only supports simple 2D graphics lol.

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
