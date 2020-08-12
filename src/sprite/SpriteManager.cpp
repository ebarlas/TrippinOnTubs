#include "SpriteManager.h"

void trippin::SpriteManager::load(SDL_Renderer *renderer, trippin::Scale sc) {
    scale = sc;
    for (auto type : spriteTypes) {
        auto name = getSpriteName(type);
        sprites[type] = std::make_unique<Sprite>(renderer, name, sc);
    }
}

const trippin::Sprite &trippin::SpriteManager::get(SpriteType type) {
    return *sprites[type];
}