#include "antumbra/types/sprite.hpp"
#include "penumbra.hpp"
#include "utils/config.hpp"
#include "utils/vectors.hpp"

#include <cmath>
#include <cstdint>
#include <string>

#ifdef __EMSCRIPTEN__
    #include <emscripten/emscripten.h>
#endif

pen::antumbra::Sprite* magic_1;
pen::antumbra::Sprite* magic;
pen::antumbra::Sprite* magic2;
pen::antumbra::Sprite* magic3;

double counter = 0;

void update(void*) {
    const double dt = pen::getDeltaTime();
    counter += dt;

    // MAGIC
    // Rotation
    magic_1->transform.rotation += dt * 1.0;
    magic->transform.rotation -= dt * 0.75;
    magic2->transform.rotation += dt * 0.5;
    magic3->transform.rotation -= dt * 0.25;
    // Position
    magic3->transform.position.x = cos(counter) * 0.02;
    magic3->transform.position.y = sin(counter) * 0.02;
    magic2->transform.position.x = cos(counter*0.5) * 0.1;
    magic2->transform.position.y = sin(counter*0.5) * 0.1;
    magic->transform.position.x = cos(counter*0.25) * 0.2;
    magic->transform.position.y = sin(counter*0.25) * 0.2;
    magic_1->transform.position.x = cos(counter*0.125) * 0.3;
    magic_1->transform.position.y = sin(counter*0.125) * 0.3;

    pen::update();
}

int main(int argc, char** argv) {
    uint32_t flags = pen::getFlagsFromArguments(argc, argv);
    // DEBUG:
    // Force Wayland
    // flags = flags | PENUMBRA_WAYLAND;

    pen::createWindow("Penumbra", 1280, 720, flags);
    pen::createAntumbra("pnmbr/shaders-2D/default");

    magic_1 = pen::createSprite("pnmbr/images/hexta.png", pen::Vec2(0, 0), 0, 0.6);
    magic = pen::createSprite("pnmbr/images/hexta.png", pen::Vec2(0, 0), 0, 1.125);
    magic2 = pen::createSprite("pnmbr/images/hexta.png", pen::Vec2(0, 0), 0, 2, "pnmbr/shaders-2D/uv-alpha");
    magic3 = pen::createSprite("pnmbr/images/hexta.png", pen::Vec2(0, 0), 0, 3.5, "pnmbr/shaders-2D/rgb-alpha");

    // Run Program
    #ifdef __EMSCRIPTEN__
        emscripten_set_main_loop_arg(update, &data, 0, true);
    #else
        while (pen::running()) {
            update(nullptr);
        }
    #endif

    pen::end();
}