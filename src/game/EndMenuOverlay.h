#ifndef TRIPPIN_ENDMENUOVERLAY_H
#define TRIPPIN_ENDMENUOVERLAY_H

#include "sprite/SpriteManager.h"

namespace trippin {
    class EndMenuOverlay {
    public:
        void init(const Point<int> &windowSize, SpriteManager &spriteManager);
        void render();
        bool exitClicked(const Point<int> &coords) const;
        bool saveClicked(const Point<int> &coords) const;
    private:
        const Sprite *saveSprite;
        const Sprite *exitSprite;
        Point<int> savePos;
        Point<int> exitPos;
    };
}

#endif
