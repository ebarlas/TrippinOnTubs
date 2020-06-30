#include "SpriteManager.h"

void trippin::SpriteManager::setScale(trippin::Scale s) {
    scale = s;
}

void trippin::SpriteManager::load(SDL_Renderer *renderer) {
    for (auto type : spriteTypes) {
        auto name = getSpriteName(type);
        sprites[type] = std::make_unique<Sprite>();
        sprites[type]->load(renderer, name, scale);
    }
}

const trippin::Sprite &trippin::SpriteManager::get(SpriteType type) {
    return *sprites[type];
}
