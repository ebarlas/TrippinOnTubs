#include "SpriteManager.h"

#include <utility>

trippin::SpriteManager::SpriteManager(SDL_Renderer *renderer, Scale scale, int tickPeriod)
        : renderer(renderer), scale(std::move(scale)), tickPeriod(tickPeriod) {

}

const trippin::Sprite &trippin::SpriteManager::get(const std::string &type) {
    auto it = sprites.find(type);
    if (it == sprites.end()) {
        return *(sprites[type] = std::make_unique<Sprite>(renderer, type, scale, tickPeriod));
    } else {
        return *it->second;
    }
}
