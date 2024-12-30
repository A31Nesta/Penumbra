# TO DO LIST

## \[General\]

- Move Build System to Meson.

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

- Fix mipmap generation on non-square/non-power-of-two sized images