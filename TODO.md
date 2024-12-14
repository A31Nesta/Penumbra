# TO DO LIST

## \[WGPU\]

### Refactor

- Refactor **Object Manager** to allow the creation of every (used) WGPU Object

- Create a "**Descriptor Options**" (or similar) file with macros or options to configure the object creation.

- Move **Layout Generation to Backend** so that it's only done once per Renderer. (Once per Pipeline Type)

- Separate Shaders into **2D and 3D Shaders**. Shaders are built after generating the layout and after uniforms are created.

- Make **Sprites** get their Bind Groups from the Object Manager. There should be 0 Backend-specific code in the Sprite code.

- **Shaders** and **Textures** being part of the rendering backend itself still makes sense, since you don't work with them directly, but they should follow the same structure