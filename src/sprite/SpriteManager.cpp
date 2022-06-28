#include "SpriteManager.h"

trippin::SpriteManager::SpriteManager(SDL_Renderer *renderer, SpriteLoader &spriteLoader, double tickPeriod)
        : tickPeriod(tickPeriod), renderer(renderer), spriteLoader(spriteLoader) {
}

const trippin::Sprite &trippin::SpriteManager::get(const std::string &type) {
    auto it = sprites.find(type);
    if (it == sprites.end()) {
        return *(sprites[type] = newSprite(type));
    } else {
        return *it->second;
    }
}

void trippin::SpriteManager::setSurfaces(std::unique_ptr<std::unordered_map<std::string, SDL_Surface *>> surf) {
    surfaces = std::move(surf);
}

trippin::SpriteManager::SpritePtr trippin::SpriteManager::newSprite(const std::string &type) {
    if (!surfaces) {
        return std::make_unique<Sprite>(renderer, type, spriteLoader, tickPeriod);
    }

    auto it = surfaces->find(type);
    if (it == surfaces->end()) {
        return std::make_unique<Sprite>(renderer, type, spriteLoader, tickPeriod);
    }

    auto surface = it->second;
    surfaces->erase(it);
    return std::make_unique<Sprite>(renderer, type, spriteLoader.getScale(), tickPeriod, surface);
}