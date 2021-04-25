#ifndef TRIPPIN_SCROLLINGSCOREBOARD_H
#define TRIPPIN_SCROLLINGSCOREBOARD_H

#include <vector>
#include "sprite/SpriteManager.h"
#include "net/Score.h"
#include "ScoreBoard.h"

namespace trippin {
    class ScrollingScoreBoard {
    public:
        ScrollingScoreBoard(const Point<int> &windowSize, double scrollRate, SpriteManager &spriteManager);
        void setScores(std::vector<Score> scores);
        void reset();
        void render();
    private:
        const Point<int> windowSize;
        ScoreBoard scoreBoard;

        const double scrollRate;
        Uint32 startTime;
        int scrollTop;
    };
}

#endif
