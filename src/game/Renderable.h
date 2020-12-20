#ifndef TRIPPIN_RENDERABLE_H
#define TRIPPIN_RENDERABLE_H

#include "sprite/Camera.h"

namespace trippin {
    class Renderable {
    public:
        virtual void render(const Camera &camera) = 0;
        virtual ~Renderable() = default;
    };
}

#endif
