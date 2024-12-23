# TO DO LIST

## \[Antumbra\]

- Rename **Sprite** to **Sprite Holder** and add the **Sprite Instance**
- Add the **Sprite** class to interface with Sprite Instances and Holders.
- Make Antumbra (2D) support Instancing.
    - Examples: [Learn WGPU (Rust)](https://sotrh.github.io/learn-wgpu/beginner/tutorial7-instancing) and [WebGPU Samples (TypeScript)](https://webgpu.github.io/webgpu-samples/?sample=instancedCube)
    - Make this configurable, maybe the user wants 5 instances, maybe they want 1000, maybe they want no instancing at all.

## \[WGPU\]

### Refactor

- Refactor **Object Manager** to manage every (used) WGPU Object (creation and destruction)
- Create a "**Descriptor Options**" (or similar) file with macros or options to configure the object creation.
- Move **Layout Generation to Backend** so that it's only done once per Renderer. (Once per Pipeline Type)
- Separate Shaders into **2D and 3D Shaders**. Shaders are built after generating the layout and after uniforms are created.
- Make **Sprites** get their Bind Groups (uniforms) from the Object Manager. There should be 0 Backend-specific code in the Sprite code.
- **Shaders** and **Textures** being part of the rendering backend itself still makes sense, since you don't work with them directly, but they should follow the same structure (review)