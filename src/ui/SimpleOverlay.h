#ifndef TRIPPIN_SIMPLEOVERLAY_H
#define TRIPPIN_SIMPLEOVERLAY_H

#include "sprite/SpriteManager.h"
#include "Interpolator.h"

namespace trippin {
    class SimpleOverlay {
    public:
        SimpleOverlay(
                const Point<int> &windowSize,
                const Sprite &sprite,
                const RenderClock &renderClock);
        void reset(int frame = 0);
        void render();
    private:
        const Sprite &sprite;
        Interpolator interpolator;
        Point<int> windowSize;
        int frame;
    };
}

#endif