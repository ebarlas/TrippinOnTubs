#include "Camera.h"

trippin::Camera::Camera(trippin::Rect<int_fast64_t> universe, Rect<int_fast64_t> viewport, int_fast64_t shakeMargin) :
        universe(universe),
        viewport(viewport),
        shakeMargin(shakeMargin) {

}

trippin::Rect<int_fast64_t> trippin::Camera::getViewport() const {
    return viewport;
}

trippin::Rect<int_fast64_t> trippin::Camera::getUniverse() const {
    return universe;
}

void trippin::Camera::centerOn(Point<int_fast64_t> center, Point<int_fast64_t> shake) {
    auto x = center.x - viewport.w / 4;
    auto y = center.y - viewport.h / 2;

    if (x < universe.x + shakeMargin) {
        x = universe.x + shakeMargin;
    } else if (x + viewport.w > universe.x + universe.w - shakeMargin) {
        x = universe.x + universe.w - viewport.w - shakeMargin;
    }

    if (y < universe.y + shakeMargin) {
        y = universe.y + shakeMargin;
    } else if (y + viewport.h > universe.y + universe.h - shakeMargin) {
        y = universe.y + universe.h - viewport.h - shakeMargin;
    }

    viewport.x = x + shake.x;
    viewport.y = y + shake.y;
}
