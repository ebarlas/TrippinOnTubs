#include <cmath>
#include "Sprite.h"

trippin::Sprite::Sprite(SDL_Renderer *ren, const std::string &name, const Scale &sc, int tickPeriod)
        : scale(sc), ren(ren), sheet(ren, name, scale) {
    metadata.load(name);

    size = sheet.getSize();
    size.x /= metadata.getFrames();

    auto mul = scale.getMultiplier();
    auto hb = metadata.getHitBox();
    hitBox = {static_cast<int>(std::round(hb.x * mul)),
              static_cast<int>(std::round(hb.y * mul)),
              static_cast<int>(std::round(hb.w * mul)),
              static_cast<int>(std::round(hb.h * mul))};

    // Duration in (milliseconds) and ticks period (milliseconds per tick)
    framePeriodTicks = metadata.getDuration() / tickPeriod;
}

void trippin::Sprite::render(trippin::Point<int> position, int frame) const {
    SDL_Rect clip{frame * size.x, 0, size.x, size.y};
    SDL_Rect target{position.x, position.y, size.x, size.y};
    sheet.render(&clip, &target);
}

void trippin::Sprite::render(trippin::Point<int> hitBoxPos, int frame, const trippin::Camera &camera) const {
    auto viewport = camera.getViewport();
    trippin::Rect<int> box{hitBoxPos.x - hitBox.x, hitBoxPos.y - hitBox.y, size.x, size.y};
    if (box.hasCollision(viewport)) {
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

int trippin::Sprite::getFrames() const {
    return metadata.getFrames();
}

int trippin::Sprite::getFramePeriodTicks() const {
    return framePeriodTicks;
}

int trippin::Sprite::getFrameDuration() const {
    return metadata.getDuration();
}

const trippin::Scale &trippin::Sprite::getScale() const {
    return scale;
}

bool trippin::Sprite::intersectsWith(Point<int> hitBoxPos, Rect<int> rect) const {
    return rect.intersect({hitBoxPos.x - hitBox.x, hitBoxPos.y - hitBox.y, size.x, size.y});
}
