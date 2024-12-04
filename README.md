# Penumbra

Simple Renderer built for Selengine.   
It features a strange and simplistic lighting model based on textures created specifically for the game being developed in Selengine.   
Cross-platform but primarily tested and developed on Linux.

It supports BGFX and WebGPU as backends (WebGPU via WebGPU-Native).

## What does it support?

- 2D and 3D Graphics via separate modules (Antumbra and Umbra respectively)
- Simple phong-based lighting
- Model loading (From Hope-Soda files or normal files)

## What may be supported in the future?

**Not planned for now**

- 2D Normal Map support for better 2D Lighting
- PBR Lighting
- Shadow Mapping (ironic that it's not supported for now)

## Is there a lore reason why it's called like that?

Yes, there is.  
There is a lore reason that somewhat relates to the main projects (Selengine and the game being made with it)

## Dependencies
### Utilities
- bgfx-shaderc (included with BGFX) \[Only if using BGFX as backend\]
- toktx

### Libraries
- BGFX \[Only if using BGFX as backend\]
- WebGPU Native \[Only if using WebGPU as backend\]
- GLFW
- GLM
- STB Image (included)

### Arch Linux Packages
- bgfx-cmake-git
- ktx_software-git
- glfw
- glm