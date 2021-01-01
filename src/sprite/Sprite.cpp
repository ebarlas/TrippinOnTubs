#include <cmath>
#include "Sprite.h"

trippin::Sprite::Sprite(SDL_Renderer *ren, const std::string &name, const Scale &scale)
        : scale(scale), sheet(ren, name, scale), ren(ren) {
    metadata.load(name);

    size = sheet.getSize();
    size.x /= metadata.getFrames();

    auto mul = scale.getMultiplier();
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

void trippin::Sprite::render(trippin::Point<int> hitBoxPos, int frame, const trippin::Camera &camera) const {
    auto viewport = camera.getViewport();
    trippin::Rect<int> box{hitBoxPos.x - hitBox.x, hitBoxPos.y - hitBox.y, size.x, size.y};
    if (box.intersect(viewport)) {
        Point<int> target = {box.x - viewport.x, box.y - viewport.y};
        render(target, frame);
        /*
        SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
        SDL_Rect r{hitBoxPos.x - viewport.x, hitBoxPos.y - viewport.y, hitBox.w, hitBox.h};
        SDL_RenderDrawRect(ren, &r);
        */
    }
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

const trippin::Scale &trippin::Sprite::getScale() const {
    return scale;
}
