#ifndef TRIPPIN_NAMEFORM_H
#define TRIPPIN_NAMEFORM_H

#include "sprite/SpriteManager.h"

namespace trippin {
    class NameForm {
    public:
        NameForm(const Point<int> &windowSize, SpriteManager &spriteManager);
        void render();
        void reset();
        void onClick(const Point<int> &coords);
        bool nameEntered() const;
        const std::string& getName() const;
    private:
        static constexpr const int chars = 26;
        static constexpr const int columns = 10;
        static constexpr const int rows = 3;
        static constexpr const int nameLength = 5;
        static constexpr const int paddingDivisor = 16;
        std::string name;
        const Sprite &sprite;
        Point<int> windowSize;
        int cursor;
    };
}

#endif
