#include "penumbra.hpp"
#include "utils/config.hpp"

#include <cstdint>
#include <string>
#include <iostream>

#ifdef __EMSCRIPTEN__
    #include <emscripten/emscripten.h>
#endif

struct Data {
    pen::antumbra::Sprite* magic_1;
    pen::antumbra::Sprite* magic;
    pen::antumbra::Sprite* magic2;
    pen::antumbra::Sprite* magic3;
};

void update(void* dataVoidPtr) {
    Data* data = reinterpret_cast<Data*>(dataVoidPtr);
    const double dt = pen::getDeltaTime();
    data->magic_1->transform.rotation += dt * 1.0;
    data->magic->transform.rotation -= dt * 0.75;
    data->magic2->transform.rotation += dt * 0.5;
    data->magic3->transform.rotation -= dt * 0.25;

    pen::update();
}

int main(int argc, char** argv) {
    uint32_t flags = pen::getFlagsFromArguments(argc, argv);
    Data data;

    // DEBUG:
    // Force Wayland
    // flags = flags | PENUMBRA_WAYLAND;

    pen::createWindow("Penumbra", 1280, 720, flags);
    pen::createAntumbra("pnmbr/shaders-2D/default");

    data.magic_1 = pen::createSprite("pnmbr/images/hexta.png", pen::Vec2(0, 0), 0, 0.6);
    data.magic = pen::createSprite("pnmbr/images/hexta.png", pen::Vec2(0, 0), 0, 1.125);
    data.magic2 = pen::createSprite("pnmbr/images/hexta.png", pen::Vec2(0, 0), 0, 2);
    data.magic3 = pen::createSprite("pnmbr/images/hexta.png", pen::Vec2(0, 0), 0, 3.5, "pnmbr/shaders-2D/rgb-alpha");

    // Run Program
    #ifdef __EMSCRIPTEN__
        emscripten_set_main_loop_arg(update, &data, 0, true);
    #else
        while (pen::running()) {
            const double dt = pen::getDeltaTime();
            data.magic_1->transform.rotation += dt * 1.0;
            data.magic->transform.rotation -= dt * 0.75;
            data.magic2->transform.rotation += dt * 0.5;
            data.magic3->transform.rotation -= dt * 0.25;

            pen::update();
        }
    #endif

    pen::end();
}