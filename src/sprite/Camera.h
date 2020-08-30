#ifndef TRIPPIN_CAMERA_H
#define TRIPPIN_CAMERA_H

#include "engine/Rect.h"

namespace trippin {
    class Camera {
    public:
        void centerOn(Point<int> point);
        void setUniverse(Rect<int> universe);
        void setViewport(Rect<int> viewport);
        bool inView(Rect<int> rect) const;
        Rect<int> getUniverse() const;
        Rect<int> getViewport() const;
    private:
        Rect<int> universe{};
        Rect<int> viewport{};
    };
}

#endif
