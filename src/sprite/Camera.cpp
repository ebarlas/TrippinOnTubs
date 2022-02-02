#include "Camera.h"

trippin::Camera::Camera(trippin::Rect<int> universe, Rect<int> viewport, int shakeMargin) :
        universe(universe),
        viewport(viewport),
        shakeMargin(shakeMargin) {

}

trippin::Rect<int> trippin::Camera::getViewport() const {
    return viewport;
}

trippin::Rect<int> trippin::Camera::getUniverse() const {
    return universe;
}

void trippin::Camera::centerOn(Point<int> center, Point<int> shake) {
    double x = center.x - viewport.w / 4.0;
    double y = center.y - viewport.h / 2.0;

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
