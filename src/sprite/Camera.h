#ifndef TRIPPIN_CAMERA_H
#define TRIPPIN_CAMERA_H

#include "engine/Rect.h"

namespace trippin {
    class Camera {
    public:
        // Universe, viewport, and shake margin are all in engine unit scale
        Camera(Rect<int_fast64_t> universe, Rect<int_fast64_t> viewport, int_fast64_t shakeMargin);
        void centerOn(Point<int_fast64_t> point, Point<int_fast64_t> shake);
        // Get universe rectangle in engine scale
        Rect<int_fast64_t> getUniverse() const;
        // Get viewport rectangle in engine scale
        Rect<int_fast64_t> getViewport() const;
    private:
        const Rect<int_fast64_t> universe;
        Rect<int_fast64_t> viewport;
        const int_fast64_t shakeMargin;
    };
}

#endif
