#ifndef TRIPPIN_SCOREBOARD_H
#define TRIPPIN_SCOREBOARD_H

#include <vector>
#include <optional>
#include "sprite/Sprite.h"
#include "net/Score.h"

namespace trippin {
    class ScoreBoard {
    public:
        ScoreBoard(const Sprite &sprite);
        void setScores(std::vector<Score> scores);
        void render(Point<int> position);
        int getHeight() const;
        int getWidth() const;
        int numScores() const;
        const Score& scoreAt(int n) const;
    private:
        std::vector<Score> scores;
        const Sprite &sprite;
        void renderName(Point<int> position, const std::string &name) const;
    };
}

#endif
