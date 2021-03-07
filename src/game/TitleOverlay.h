#ifndef TRIPPIN_TITLEOVERLAY_H
#define TRIPPIN_TITLEOVERLAY_H

#include "sprite/SpriteManager.h"

namespace trippin {
    class TitleOverlay {
    public:
        void init(const Point<int> &windowSize, SpriteManager &spriteManager);
        void render();
    private:
        const Sprite *sprite;
        Point<int> position;
    };
}

#endif
