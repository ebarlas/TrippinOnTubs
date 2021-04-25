#ifndef TRIPPIN_ENDMENU_H
#define TRIPPIN_ENDMENU_H

#include "sprite/SpriteManager.h"
#include "ui/MenuLayout.h"

namespace trippin {
    class EndMenu {
    public:
        EndMenu(const Point<int> &windowSize, SpriteManager &spriteManager);
        void reset();
        void render();
        bool exitClicked(const Point<int> &coords) const;
        bool saveClicked(const Point<int> &coords) const;
    private:
        MenuLayout<2> menuLayout;
    };
}

#endif
