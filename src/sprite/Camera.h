#ifndef TRIPPIN_CAMERA_H
#define TRIPPIN_CAMERA_H

#include "engine/Rect.h"

namespace trippin {
    class Camera {
    public:
        void centerOn(Vector<int> point);
        void setUniverse(Rect<int> universe);
        void setViewport(Rect<int> viewport);
        bool inView(const Rect<int> &rect) const;
        const Rect<int> &getUniverse() const;
        const Rect<int> &getViewport() const;
    private:
        Rect<int> universe{};
        Rect<int> viewport{};
    };
}

#endif
