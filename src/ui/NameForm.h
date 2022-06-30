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
        const std::string &getName() const;
    private:
        static constexpr const int columns = 10;
        static constexpr const int rows = 3;
        static constexpr const int nameLength = 3;
        static constexpr const int paddingDivisor = 16;
        std::string name;
        // 40 frames arranged as follows: [10 digits][26 letters][1 delete][1 enter][1 box][1 underscore]
        const Sprite &sprite;
        const Point<int> windowSize;
        const Point<int> gridSize;
        const int padding;
        int cursor;
        bool entered;
        static Point<int> computeGridSize(Point<int> windowSize, Point<int> spriteSize);
    };
}

#endif
