#ifndef TRIPPIN_NAMEFORMOVERLAY_H
#define TRIPPIN_NAMEFORMOVERLAY_H

#include "sprite/SpriteManager.h"
#include "Name.h"

namespace trippin {
    class NameFormOverlay {
    public:
        void init(const Point<int> &windowSize, SpriteManager &spriteManager);
        void render();
        void reset();
        void onClick(const Point<int> &coords);
        bool nameEntered() const;
        const Name& getName() const;
    private:
        static constexpr const int chars = 26;
        static constexpr const int columns = 10;
        static constexpr const int rows = 3;
        static const int nameLength = 5;
        Name name;
        const Sprite *sprite;
        Point<int> windowSize;
        int cursor;
    };
}

#endif
