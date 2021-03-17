#ifndef TRIPPIN_MENUOVERLAY_H
#define TRIPPIN_MENUOVERLAY_H

#include "sprite/SpriteManager.h"
#include "MenuLayout.h"

namespace trippin {
    class MenuOverlay {
    public:
        void init(const Point<int> &windowSize, SpriteManager &spriteManager);
        void reset();
        void render();
        bool startClicked(const Point<int> &coords) const;
        bool exitClicked(const Point<int> &coords) const;
        bool highScoreClicked(const Point<int> &coords) const;
    private:
        MenuLayout<3> menuLayout;
    };
}

#endif
