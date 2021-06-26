#ifndef TRIPPIN_TITLEOVERLAY_H
#define TRIPPIN_TITLEOVERLAY_H

#include "sprite/SpriteManager.h"
#include "sprite/RenderClock.h"
#include "ui/Interpolator.h"
#include "net/Score.h"
#include "ui/ScoreBoard.h"

namespace trippin {
    class TitleOverlay {
    public:
        struct Options {
            double scrollRate;
            int titlePause;
        };

        TitleOverlay(
                const Point<int> &windowSize,
                Options options,
                SpriteManager &spriteManager,
                const RenderClock &renderClock);
        void setScores(std::vector<Score> today, std::vector<Score> top);
        void render();
        bool hasScores() const;
    private:
        const Sprite &titleSprite;
        const Sprite &allTimeSprite;
        const Sprite &todaySprite;
        const RenderClock &renderClock;
        Point<int> windowSize;
        ScoreBoard todayScoreBoard;
        ScoreBoard allTimeScoreBoard;

        bool scoresSet;
        Uint32 scoresSetTicks;

        const Options options;

        int getScrollTop() const;
    };
}

#endif
