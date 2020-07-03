#include <cmath>
#include "Sprite.h"

void trippin::Sprite::load(SDL_Renderer *renderer, const std::string &name, Scale scale) {
    metadata.load(name);
    sheet.load(renderer, name, scale);

    size = sheet.getSize();
    size.x /= metadata.getFrames();

    auto mul = scaleMultiplier(scale);
    auto &hb = metadata.getHitBox();
    hitBox = {static_cast<int>(std::round(hb.x * mul)),
              static_cast<int>(std::round(hb.y * mul)),
              static_cast<int>(std::round(hb.w * mul)),
              static_cast<int>(std::round(hb.h * mul))};
}

void trippin::Sprite::render(SDL_Renderer *renderer, const trippin::Vector<int> position, int frame) const {
    SDL_Rect clip{frame * size.x, 0, size.x, size.y};
    SDL_Rect target{position.x, position.y, size.x, size.y};
    sheet.render(renderer, &clip, &target);
}

const trippin::Vector<int> &trippin::Sprite::getSize() const {
    return size;
}

const trippin::Rect<int> &trippin::Sprite::getHitBox() const {
    return hitBox;
}

int trippin::Sprite::getDuration() const {
    return metadata.getDuration();
}

int trippin::Sprite::getFrames() const {
    return metadata.getFrames();
}
