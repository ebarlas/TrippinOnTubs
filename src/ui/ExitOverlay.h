#ifndef TRIPPIN_EXITOVERLAY_H
#define TRIPPIN_EXITOVERLAY_H

#include "sprite/SpriteManager.h"
#include "ui/Interpolator.h"

namespace trippin {
    class ExitOverlay {
    public:
        ExitOverlay(
                const Point<int> &windowSize,
                int margin,
                SpriteManager &spriteManager,
                const RenderClock &renderClock);
        void reset();
        void render();
        bool exitClicked(const Point<int> &coords) const;
    private:
        const Sprite &exitSprite;
        const int top;
        Interpolator interpolator;
    };
}

#endif
