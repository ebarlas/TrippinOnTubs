#ifndef TRIPPIN_SCROLLINGSCOREBOARDOVERLAY_H
#define TRIPPIN_SCROLLINGSCOREBOARDOVERLAY_H

#include <vector>
#include "sprite/SpriteManager.h"
#include "net/Score.h"
#include "ScoreBoardOverlay.h"

namespace trippin {
    class ScrollingScoreBoardOverlay {
    public:
        void setScores(std::vector<Score> scores);
        void setScrollRate(double scrollRate);
        void init(const Point<int> &windowSize, SpriteManager &spriteManager);
        void reset();
        void render();
    private:
        Point<int> windowSize;
        ScoreBoardOverlay scoreBoard;

        double scrollRate;
        Uint32 startTime;

        void scroll();
        void applyScroll(int delta);
    };
}

#endif
