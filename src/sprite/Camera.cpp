#include "Camera.h"

void trippin::Camera::setViewport(Rect<int> vp) {
    viewport = vp;
}

trippin::Rect<int> trippin::Camera::getViewport() const {
    return viewport;
}

void trippin::Camera::setUniverse(Rect<int> uni) {
    universe = uni;
}

trippin::Rect<int> trippin::Camera::getUniverse() const {
    return universe;
}

void trippin::Camera::centerOn(Point<int> center) {
    double x = center.x - viewport.w / 4.0;
    double y = center.y - viewport.h / 2.0;

    if (x < universe.x) {
        x = universe.x;
    } else if (x + viewport.w > universe.x + universe.w) {
        x = universe.x + universe.w - viewport.w;
    }

    if (y < universe.y) {
        y = universe.y;
    } else if (y + viewport.h > universe.y + universe.h) {
        y = universe.y + universe.h - viewport.h;
    }

    viewport.x = x;
    viewport.y = y;
}

bool trippin::Camera::inView(Rect<int> rect) const {
    return viewport.intersect(rect);
}
