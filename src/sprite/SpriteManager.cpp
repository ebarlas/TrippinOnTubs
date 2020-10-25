#include "SpriteManager.h"

trippin::SpriteManager::SpriteManager(SDL_Renderer *renderer, const Scale &scale)
        : renderer(renderer), scale(scale) {

}

const trippin::Sprite &trippin::SpriteManager::get(const std::string &type) {
    auto it = sprites.find(type);
    if (it == sprites.end()) {
        return *(sprites[type] = std::make_unique<Sprite>(renderer, type, scale));
    } else {
        return *it->second;
    }
}