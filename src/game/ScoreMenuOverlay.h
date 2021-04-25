#ifndef TRIPPIN_SCOREMENUOVERLAY_H
#define TRIPPIN_SCOREMENUOVERLAY_H

#include "sprite/SpriteManager.h"
#include "ui/MenuLayout.h"

namespace trippin {
    class ScoreMenuOverlay {
    public:
        void init(const Point<int> &windowSize, SpriteManager &spriteManager);
        void reset();
        void render();
        bool allTimeClicked(const Point<int> &coords) const;
        bool todayClicked(const Point<int> &coords) const;
        bool exitClicked(const Point<int> &coords) const;
    private:
        MenuLayout<3> menuLayout;
    };
}

#endif
