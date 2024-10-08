#pragma once

/**
    ANTUMBRA
    ---------------------------------------------------
    2D Renderer. It holds data necessary for rendering.

    The user can register, unregister and modify (through interfaces):
    - Sprites: A struct containing:
        - A Transform: Position, Rotation and Scale
        - A Deform: Scale applied to the original 1x1 sprite. It corrects Aspect Ratio to match the texture
        - Private: A Texture ID
    
    The renderer automatically manages:
    - Textures: A struct containing:
        - A path
        - A handle from the graphics library
        - The number of sprites using it. If no sprite is using it, it will get deleted
        - A Persistence flag. A boolean value indicating whether or not it will get deleted when it has no users
        
 */

namespace pen::antumbra {
    class Antumbra {
    public:
        Antumbra();
        
    private:

    };
}