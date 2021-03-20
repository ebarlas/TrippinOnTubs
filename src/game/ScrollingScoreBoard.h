#ifndef TRIPPIN_SCROLLINGSCOREBOARD_H
#define TRIPPIN_SCROLLINGSCOREBOARD_H

#include <vector>
#include "sprite/SpriteManager.h"
#include "Score.h"
#include "ScoreBoard.h"

namespace trippin {
    class ScrollingScoreBoard {
    public:
        void setScores(std::vector<Score> scores);
        void setScrollRate(double scrollRate);
        void init(const Point<int> &windowSize, SpriteManager &spriteManager);
        void reset();
        void render();
    private:
        Point<int> windowSize;
        ScoreBoard scoreBoard;

        double scrollRate;
        Uint32 startTime;

        void scroll();
        void applyScroll(int delta);
    };
}

#endif
