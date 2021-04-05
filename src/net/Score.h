#ifndef TRIPPIN_SCORE_H
#define TRIPPIN_SCORE_H

namespace trippin {
    struct Score {
        int score;
        std::string name;
        std::string id;

        bool operator==(const Score &ds) const {
            return score == ds.score && name == ds.name && id == ds.id;
        }
    };
}

#endif
