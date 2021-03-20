#ifndef TRIPPIN_SCOREBOARD_H
#define TRIPPIN_SCOREBOARD_H

#include <vector>
#include "sprite/SpriteManager.h"
#include "Score.h"

namespace trippin {
    class ScoreBoard {
    public:
        void setScores(std::vector<Score> scores);
        void setTop(int y);
        int getTop() const;
        void init(const Point<int> &windowSize, SpriteManager &spriteManager);
        void render();
        int getHeight() const;
    private:
        std::vector<Score> scores;
        const Sprite *sprite;
        Point<int> windowSize;
        int top;
        void renderNumber(int x, int y, int value);
        void renderName(int x, int y, const std::string &name);
    };
}

#endif
