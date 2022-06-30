#ifndef TRIPPIN_SCORE_H
#define TRIPPIN_SCORE_H

namespace trippin {
    struct Score {
        struct InputEvent {
            int tick;
            int input;
        };

        std::string id;
        int game;
        int score;
        std::string name;
        std::vector<InputEvent> events;

        bool operator==(const Score &ds) const {
            return id == ds.id && game == ds.game;
        }
    };
}

#endif
