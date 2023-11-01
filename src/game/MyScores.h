#ifndef TRIPPIN_MYSCORES_H
#define TRIPPIN_MYSCORES_H

#include <vector>
#include "net/Score.h"

namespace trippin {
    class MyScores {
    public:
        enum class Type {
            top,
            latest
        };

        MyScores(Type type, int version, int limit);
        void addScore(const Score &score);
        std::vector<Score> getScores() const;
    private:
        const Type type;
        const int version;
        const int limit;
        std::vector<Score> scores;

        std::string fileName() const;
    };
}

#endif
