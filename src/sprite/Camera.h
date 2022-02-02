#ifndef TRIPPIN_CAMERA_H
#define TRIPPIN_CAMERA_H

#include "engine/Rect.h"

namespace trippin {
    class Camera {
    public:
        Camera(Rect<int> universe, Rect<int> viewport, int shakeMargin);
        void centerOn(Point<int> point, Point<int> shake);
        Rect<int> getUniverse() const;
        Rect<int> getViewport() const;
    private:
        const Rect<int> universe;
        Rect<int> viewport;
        const int shakeMargin;
    };
}

#endif
