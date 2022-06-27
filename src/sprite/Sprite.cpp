#include <cmath>
#include "Sprite.h"

trippin::Sprite::Sprite(SDL_Renderer *renderer, const std::string &name, const SpriteLoader &loader, double tickPeriod)
        : scale(loader.getScale()), ren(renderer), sheet(renderer, name, loader) {
    init(name, tickPeriod);
}

trippin::Sprite::Sprite(SDL_Renderer *ren, const std::string &name, const Scale &sc, double tickPeriod, SDL_Surface *sur)
        : scale(sc), ren(ren), sheet(ren, sur) {
    init(name, tickPeriod);
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

void trippin::Sprite::renderDevice(trippin::Point<int> position, int frame) const {
    SDL_Rect clip{frame * deviceSize.x, 0, deviceSize.x, deviceSize.y};
    SDL_Rect target{position.x, position.y, deviceSize.x, deviceSize.y};
    sheet.render(&clip, &target);
}

void trippin::Sprite::renderEngine(trippin::Point<int> hitBoxPos, int frame, const trippin::Camera &camera) const {
    auto viewport = camera.getViewport();
    trippin::Rect<int> box{hitBoxPos.x - engineHitBox.x, hitBoxPos.y - engineHitBox.y, engineSize.x, engineSize.y};
    if (box.hasCollision(viewport)) {
        Point<int> target = {box.x - viewport.x, box.y - viewport.y};
        renderDevice(target / scale.getDeviceEngineFactor(), frame);
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