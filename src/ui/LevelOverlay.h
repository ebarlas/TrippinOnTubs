#ifndef TRIPPIN_LEVELOVERLAY_H
#define TRIPPIN_LEVELOVERLAY_H

#include "sprite/SpriteManager.h"
#include "Interpolator.h"

namespace trippin {
    class LevelOverlay {
    public:
        LevelOverlay(
                const Point<int> &windowSize, // sprite scale
                SpriteManager &spriteManager,
                const RenderClock &renderClock);
        void setLevel(int level);
        void render();
    private:
        const Sprite &sprite;
        Interpolator interpolator;
        Point<int> windowSize;
        int level;
    };
}

#endif
