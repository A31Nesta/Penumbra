# TO DO LIST

## \[WGPU\]

### Refactor

- Refactor **Object Manager** to allow the creation of every (used) WGPU Object

- Create a "**Descriptor Options**" (or similar) file with macros or options to configure the object creation.

- Move **Layout Generation to Backend** so that it's only done once per Renderer. (Once per Pipeline Type)

- Separate Shaders into **2D and 3D Shaders**. Shaders are built after generating the layout and after uniforms are created.

- Create **Default Uniforms** for Perspective, View and Model matrices

- Make WGPU Uniform Creation work like in BGFX with just a string and type. These uniforms are set from the renderer