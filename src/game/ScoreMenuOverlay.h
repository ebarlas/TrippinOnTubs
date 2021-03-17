#ifndef TRIPPIN_SCOREMENUOVERLAY_H
#define TRIPPIN_SCOREMENUOVERLAY_H

#include "sprite/SpriteManager.h"
#include "MenuLayout.h"

namespace trippin {
    class ScoreMenuOverlay {
    public:
        void init(const Point<int> &windowSize, SpriteManager &spriteManager);
        void reset();
        void render();
        bool allTimeClicked(const Point<int> &coords) const;
        bool todayClicked(const Point<int> &coords) const;
    private:
        MenuLayout<2> menuLayout;
//        const Sprite *allTimeSprite;
//        const Sprite *todaySprite;
//        Point<int> allTimePos;
//        Point<int> todayPos;
//        Interpolator interpolator;
    };
}

#endif
