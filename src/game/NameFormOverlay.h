#ifndef TRIPPIN_NAMEFORMOVERLAY_H
#define TRIPPIN_NAMEFORMOVERLAY_H

#include "sprite/SpriteManager.h"

namespace trippin {
    class NameFormOverlay {
    public:
        void init(const Point<int> &windowSize, SpriteManager &spriteManager);
        void render();
        void onClick(const Point<int> &coords);
        bool nameEntered() const;
        std::string name() const;
    private:
        static constexpr const char *alphabet = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        static constexpr const int chars = 36;
        static constexpr const int columns = 12;
        static constexpr const int rows = 3;
        static const int nameLength = 5;
        int digits[nameLength];
        const Sprite *sprite;
        Point<int> windowSize;
        int cursor;
    };
}

#endif
