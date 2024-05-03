#ifndef TRIPPIN_MYSCORES_H
#define TRIPPIN_MYSCORES_H

#include <vector>
#include "net/Channel.h"
#include "net/Score.h"

namespace trippin {
    class MyScores {
    public:
        MyScores(int version, unsigned long limit);
        void addScore(const Score &score);
        [[nodiscard]] std::vector<Score> getLatestScores() const;
        [[nodiscard]] std::vector<Score> getTopScores() const;
    private:
        struct Scores {
            std::string filename;
            std::vector<Score> scores;
        };

        const int version;
        const unsigned long limit;
        Scores latestScores;
        Scores topScores;

        static std::string fileName(std::string_view type, int version);
        static void loadScores(Scores &scores);
        void addLatestScore(const trippin::Score &score);
        void addTopScore(const trippin::Score &score);
        void resizeAndStore(std::string &filename, std::vector<Score> &scores) const;
    };
}

#endif
