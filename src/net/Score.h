#ifndef TRIPPIN_SCORE_H
#define TRIPPIN_SCORE_H

namespace trippin {
    struct Score {
        int score;
        int id;
        std::string name;

        bool operator==(const Score &ds) const {
            return score == ds.score && name == ds.name && id == ds.id;
        }
    };
}

#endif
