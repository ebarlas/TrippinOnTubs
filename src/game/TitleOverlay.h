#ifndef TRIPPIN_TITLEOVERLAY_H
#define TRIPPIN_TITLEOVERLAY_H

#include "sprite/SpriteManager.h"
#include "Interpolator.h"
#include "Score.h"
#include "ScoreBoard.h"

namespace trippin {
    class TitleOverlay {
    public:
        void setTodayScores(std::vector<Score> scores);
        void setAllTimeScores(std::vector<Score> scores);
        void setScrollRate(double scrollRate);
        void setTitlePause(int duration);
        void init(const Point<int> &windowSize, SpriteManager &spriteManager);
        void render();
    private:
        const Sprite *titleSprite;
        const Sprite *allTimeSprite;
        const Sprite *todaySprite;
        Point<int> titlePosition;
        Point<int> todayPosition;
        Point<int> allTimePosition;
        Point<int> windowSize;
        ScoreBoard todayScoreBoard;
        ScoreBoard allTimeScoreBoard;

        double scrollRate;
        Uint32 startTime;
        int titlePause;
        int height;

        void scroll();
        void applyScroll(int delta);
        bool inView(int top, int height) const;
    };
}

#endif
