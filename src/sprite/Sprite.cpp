#include <cmath>
#include "Sprite.h"

trippin::Sprite::Sprite(SDL_Renderer *renderer, const std::string &name, const SpriteLoader &loader, double tickPeriod)
        : scale(loader.getScale()), sheet(renderer, name, loader), ren(renderer) {
    init(name, tickPeriod);
}

trippin::Sprite::Sprite(SDL_Renderer *ren, const std::string &name, const Scale &sc, double tickPer, SDL_Surface *sur)
        : scale(sc), sheet(ren, sur), ren(ren) {
    init(name, tickPer);
}

void trippin::Sprite::init(const std::string &name, double tickPeriod) {
    metadata.load(name);

    deviceSize = sheet.getSize();
    deviceSize.x /= metadata.getFrames();
    engineSize = deviceSize * scale.getDeviceEngineFactor();
    engineHitBox = metadata.getHitBox() * scale.getEngineFactor();

    // Duration in (milliseconds) and ticks period (milliseconds per tick)
    framePeriodTicks = static_cast<int>(std::round(metadata.getDuration() / tickPeriod));
}

void trippin::Sprite::renderDevice(trippin::Point<int> position, int frame, double rescale) const {
    SDL_Rect clip{frame * deviceSize.x, 0, deviceSize.x, deviceSize.y};
    auto w = static_cast<int>(deviceSize.x * rescale);
    auto h = static_cast<int>(deviceSize.y * rescale);
    auto x = position.x + (deviceSize.x - w) / 2;
    auto y = position.y + (deviceSize.y - h) / 2;
    SDL_Rect target{x, y, w, h};
    sheet.render(&clip, &target);
}

void trippin::Sprite::renderEngine(trippin::Point<int> hitBoxPos, int frame, const Rect<int> &viewport, double rescale) const {
    Rect<int> box{hitBoxPos.x - engineHitBox.x, hitBoxPos.y - engineHitBox.y, engineSize.x, engineSize.y};
    if (box.hasCollision(viewport)) {
        Point<int> target = {box.x - viewport.x, box.y - viewport.y};
        renderDevice(target / scale.getDeviceEngineFactor(), frame, rescale);
        /*
        SDL_SetRenderDrawColor(ren, 0, 0, 255, 255);
        SDL_Rect r{hitBoxPos.x - viewport.x, hitBoxPos.y - viewport.y, hitBox.w, hitBox.h};
        SDL_RenderDrawRect(ren, &r);
        */
    }
}

trippin::Point<int> trippin::Sprite::getEngineSize() const {
    return engineSize;
}

trippin::Point<int> trippin::Sprite::getDeviceSize() const {
    return deviceSize;
}

trippin::Rect<int> trippin::Sprite::getEngineHitBox() const {
    return engineHitBox;
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

SDL_Renderer *trippin::Sprite::getRenderer() const {
    return ren;
}

void trippin::Sprite::advanceFrame(int ticks, int &frame, bool flash) const {
    if (ticks % framePeriodTicks == 0) {
        frame = (frame + 1) % (flash ? metadata.getFrames() / 2 : metadata.getFrames());
    }
}
