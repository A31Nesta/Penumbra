#include "penumbra.hpp"
#include "utils/config.hpp"

#include <cstdint>
#include <string>

int main(int argc, char** argv) {
    uint32_t flags = pen::getFlagsFromArguments(argc, argv);

    // DEBUG:
    // Force Wayland
    // flags = flags | PENUMBRA_WAYLAND;

    pen::createWindow("Penumbra", 1280, 720, flags);
    pen::createAntumbra("pnmbr/shaders-2D/default");

    pen::antumbra::Sprite* magic_1 = pen::createSprite("pnmbr/images/hexta.png", pen::Vec2(0, 0), 0, 0.6);
    pen::antumbra::Sprite* magic = pen::createSprite("pnmbr/images/hexta.png", pen::Vec2(0, 0), 0, 1.125);
    pen::antumbra::Sprite* magic2 = pen::createSprite("pnmbr/images/hexta.png", pen::Vec2(0, 0), 0, 2);
    pen::antumbra::Sprite* magic3 = pen::createSprite("pnmbr/images/hexta.png", pen::Vec2(0, 0), 0, 3.5, "pnmbr/shaders-2D/rgb-alpha");

    magic3->transform.position.x = 2;
    magic3->transform.scale.y = 2;

    // Run Program
    while (pen::running()) {
        const double dt = pen::getDeltaTime();
        magic_1->transform.rotation += dt * 1.0;
        magic->transform.rotation -= dt * 0.75;
        magic2->transform.rotation += dt * 0.5;
        magic3->transform.rotation -= dt * 0.25;

        pen::update();
    }

    pen::end();
}