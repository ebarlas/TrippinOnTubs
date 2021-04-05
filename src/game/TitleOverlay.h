#ifndef TRIPPIN_TITLEOVERLAY_H
#define TRIPPIN_TITLEOVERLAY_H

#include "sprite/SpriteManager.h"
#include "Interpolator.h"
#include "net/Score.h"
#include "ScoreBoardOverlay.h"

namespace trippin {
    class TitleOverlay {
    public:
        void setScores(std::vector<Score> today, std::vector<Score> top);
        void setScrollRate(double scrollRate);
        void setTitlePause(int duration);
        void init(const Point<int> &windowSize, SpriteManager &spriteManager);
        void render();
        bool hasScores() const;
    private:
        const Sprite *titleSprite;
        const Sprite *allTimeSprite;
        const Sprite *todaySprite;
        Point<int> titlePosition;
        Point<int> todayPosition;
        Point<int> allTimePosition;
        Point<int> windowSize;
        ScoreBoardOverlay todayScoreBoard;
        ScoreBoardOverlay allTimeScoreBoard;

        bool scoresSet;
        Uint32 scoresSetTicks;

        double scrollRate;
        int titlePause;

        void scroll();
        void applyScroll(int delta);
        bool inView(int top, int height) const;
    };
}

#endif
