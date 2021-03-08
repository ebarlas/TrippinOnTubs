#ifndef TRIPPIN_MENUOVERLAY_H
#define TRIPPIN_MENUOVERLAY_H

#include "sprite/SpriteManager.h"

namespace trippin {
    class MenuOverlay {
    public:
        void init(const Point<int> &windowSize, SpriteManager &spriteManager);
        void render();
        bool startClicked(const Point<int> &coords) const;
        bool exitClicked(const Point<int> &coords) const;
    private:
        const Sprite *startSprite;
        const Sprite *highScoreSprite;
        const Sprite *exitSprite;
        Point<int> startPos;
        Point<int> highScorePos;
        Point<int> exitPos;
    };
}

#endif
