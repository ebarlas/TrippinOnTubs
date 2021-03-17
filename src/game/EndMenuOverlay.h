#ifndef TRIPPIN_ENDMENUOVERLAY_H
#define TRIPPIN_ENDMENUOVERLAY_H

#include "sprite/SpriteManager.h"
#include "MenuLayout.h"

namespace trippin {
    class EndMenuOverlay {
    public:
        void init(const Point<int> &windowSize, SpriteManager &spriteManager);
        void reset();
        void render();
        bool exitClicked(const Point<int> &coords) const;
        bool saveClicked(const Point<int> &coords) const;
    private:
        MenuLayout<2> menuLayout;
    };
}

#endif
