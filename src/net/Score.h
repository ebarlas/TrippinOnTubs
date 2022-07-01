#ifndef TRIPPIN_SCORE_H
#define TRIPPIN_SCORE_H

namespace trippin {
    struct Score {
        struct InputEvent {
            const static int BIT_JUMP_CHARGE = 1;
            const static int BIT_JUMP_RELEASE = 1 << 1;
            const static int BIT_DUCK_START = 1 << 2;
            const static int BIT_DUCK_END = 1 << 3;

            int tick;
            int input;

            static InputEvent fromFlags(int tick, bool jumpCharge, bool jumpRelease, bool duckStart, bool duckEnd) {
                int input = 0;
                if (jumpCharge)
                    input |= BIT_JUMP_CHARGE;
                if (jumpRelease)
                    input |= BIT_JUMP_RELEASE;
                if (duckStart)
                    input |= BIT_DUCK_START;
                if (duckEnd)
                    input |= BIT_DUCK_END;
                return {tick, input};
            }
        };

        std::string id;
        int game;
        int score;
        std::string name;
        std::vector<std::vector<InputEvent>> events;

        bool operator==(const Score &ds) const {
            return id == ds.id && game == ds.game;
        }
    };
}

#endif
