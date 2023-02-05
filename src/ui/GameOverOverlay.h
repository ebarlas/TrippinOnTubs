#ifndef TRIPPIN_GAMEOVEROVERLAY_H
#define TRIPPIN_GAMEOVEROVERLAY_H

#include "sprite/SpriteManager.h"
#include "Interpolator.h"

namespace trippin {
    class GameOverOverlay {
    public:
        GameOverOverlay(
                const Point<int> &windowSize,
                const Sprite &sprite,
                const RenderClock &renderClock);
        void reset();
        void render();
    private:
        const Sprite &sprite;
        Interpolator interpolator;
        Point<int> windowSize;
    };
}

#endif