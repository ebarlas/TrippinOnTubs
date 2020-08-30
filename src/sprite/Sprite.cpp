#include <cmath>
#include "Sprite.h"

trippin::Sprite::Sprite(SDL_Renderer *ren, const std::string &name, trippin::Scale scale)
        : scale(scale), sheet(ren, name, scale) {
    metadata.load(name);

    size = sheet.getSize();
    size.x /= metadata.getFrames();

    auto mul = scaleMultiplier(scale);
    auto hb = metadata.getHitBox();
    hitBox = {static_cast<int>(std::round(hb.x * mul)),
              static_cast<int>(std::round(hb.y * mul)),
              static_cast<int>(std::round(hb.w * mul)),
              static_cast<int>(std::round(hb.h * mul))};
}

void trippin::Sprite::render(trippin::Point<int> position, int frame) const {
    SDL_Rect clip{frame * size.x, 0, size.x, size.y};
    SDL_Rect target{position.x, position.y, size.x, size.y};
    sheet.render(&clip, &target);
}

trippin::Point<int> trippin::Sprite::getSize() const {
    return size;
}

trippin::Rect<int> trippin::Sprite::getHitBox() const {
    return hitBox;
}

int trippin::Sprite::getDuration() const {
    return metadata.getDuration();
}

int trippin::Sprite::getFrames() const {
    return metadata.getFrames();
}

trippin::Scale trippin::Sprite::getScale() const {
    return scale;
}
