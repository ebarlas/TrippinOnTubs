#include "Sprite.h"

void trippin::Sprite::load(SDL_Renderer *renderer, const std::string &name, Scale scale) {
    metadata.load(name);
    sheet.load(renderer, name, scale);
}

void trippin::Sprite::render(SDL_Renderer *renderer, const trippin::Vector<int> position, int frame) const {
    auto size = getSize();
    SDL_Rect clip{frame * size.x, 0, size.x, size.y};
    SDL_Rect target{position.x, position.y, size.x, size.y};
    sheet.render(renderer, &clip, &target);
}

trippin::Vector<int> trippin::Sprite::getSize() const {
    auto size = sheet.getSize();
    size.x /= metadata.getFrames();
    return size;
}
