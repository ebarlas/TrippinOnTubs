#include "Sprite.h"

trippin::Sprite::Sprite(
        SDL_Renderer *renderer,
        const std::string &name,
        const Units &units,
        Fraction<int> tickPeriod,
        const SpriteLoader &loader) :
        ren(renderer),
        units(units),
        sheet(renderer, name, loader) {
    init(name, tickPeriod);
}

trippin::Sprite::Sprite(
        SDL_Renderer *ren,
        const std::string &name,
        const Units &units,
        Fraction<int> tickPeriod,
        SDL_Surface *sur) :
        ren(ren),
        units(units),
        sheet(ren, sur) {
    init(name, tickPeriod);
}

void trippin::Sprite::init(const std::string &name, Fraction<int> tickPeriod) {
    metadata.load(name);

    size = sheet.getSize();
    size.x /= metadata.getFrames(); // size per frame

    hitBox = units.baseToSprite(metadata.getHitBox());

    // Duration in (milliseconds) and ticks period (milliseconds per tick)
    framePeriodTicks = static_cast<int>(tickPeriod.flip() * metadata.getDuration());
}

void trippin::Sprite::render(Point<int> position, int frame) const {
    SDL_Rect clip{frame * size.x, 0, size.x, size.y};
    SDL_Rect target{position.x, position.y, size.x, size.y};
    sheet.render(&clip, &target);
}

void trippin::Sprite::render(Point<int_fast64_t> hitBoxPos, int frame, const Camera &camera) const {
    auto viewport = camera.getViewport();
    auto hbEngScale = units.spriteToEngine(hitBox);
    auto sizeEngScale = units.spriteToEngine(size);
    Rect<int_fast64_t> box{hitBoxPos.x - hbEngScale.x, hitBoxPos.y - hbEngScale.y, sizeEngScale.x, sizeEngScale.y};
    if (box.hasCollision(viewport)) {
        auto target = units.engineToSprite(Point<int_fast64_t> {box.x - viewport.x, box.y - viewport.y});
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

SDL_Renderer *trippin::Sprite::getRenderer() const {
    return ren;
}