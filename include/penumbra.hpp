#pragma once

#include "antumbra/types/sprite.hpp"
#include <cstdint>
#include <string>

namespace pen {
    void createWindow(std::string title, int width, int height, uint32_t penumbra_flags);
    void createAntumbra(std::string defaultShader);
    void update();
    void end();
    
    bool running();

    // Interact with windows and renderers

    // WINDOW
    double getDeltaTime();

    // ANTUMBRA
    antumbra::Sprite* createSprite(std::string texture, Transform2D transform);
    antumbra::Sprite* createSprite(std::string texture, Transform2D transform, std::string shader);
    antumbra::Sprite* createSprite(std::string texture, Vec2 position, double rotation = 0, Vec2 scale = Vec2(1));
    antumbra::Sprite* createSprite(std::string texture, Vec2 position, double rotation, Vec2 scale, std::string shader);
}