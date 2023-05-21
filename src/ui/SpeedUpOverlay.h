#ifndef TRIPPIN_SPEEDUPOVERLAY_H
#define TRIPPIN_SPEEDUPOVERLAY_H

#include "sprite/SpriteManager.h"
#include "ui/Interpolator.h"

namespace trippin {
    class SpeedUpOverlay {
    public:
        SpeedUpOverlay(
                const Point<int> &windowSize,
                int margin,
                SpriteManager &spriteManager,
                const RenderClock &renderClock);
        void reset();
        void render();
    private:
        const Sprite &sprite;
        const int top;
        Interpolator interpolator;
    };
}

#endif
