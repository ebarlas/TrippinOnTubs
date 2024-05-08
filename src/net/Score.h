#ifndef TRIPPIN_SCORE_H
#define TRIPPIN_SCORE_H

#include "nlohmann/json.hpp"

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

            bool jumpCharge() const {
                return input & BIT_JUMP_CHARGE;
            }

            bool jumpRelease() const {
                return input & BIT_JUMP_RELEASE;
            }

            bool duckStart() const {
                return input & BIT_DUCK_START;
            }

            bool duckEnd() const {
                return input & BIT_DUCK_END;
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

        static std::vector<int> compress(const std::vector<InputEvent> &events) {
            std::vector<int> v;
            v.reserve(events.size() * 2);
            for (auto &e: events) {
                v.push_back(e.tick);
                v.push_back(e.input);
            }
            return v;
        }

        std::vector<std::vector<int>> compress() const {
            std::vector<std::vector<int>> v;
            v.reserve(events.size());
            for (auto &vec: events) {
                v.push_back(compress(vec));
            }
            return v;
        }

        nlohmann::json to_json() const {
            nlohmann::json j;
            j["id"] = id;
            j["game"] = game;
            j["name"] = name;
            j["score"] = score;
            j["events"] = compress();
            return j;
        }
    };

    inline void to_json(nlohmann::json &j, const Score &score) {
        j = score.to_json();
    }

    // this function assumes diff-compressed form
    inline void from_json(const nlohmann::json &j, std::vector<Score::InputEvent> &points) {
        int lastTick = 0;
        for (auto i = j.cbegin(); i != j.cend(); i++) {
            int tickDiff = *i;
            lastTick += tickDiff;
            if (++i != j.cend()) {
                points.push_back({lastTick, *i});
            }
        }
    }

    inline void from_json(const nlohmann::json &j, trippin::Score &score) {
        j.at("id").get_to(score.id);
        j.at("game").get_to(score.game);
        j.at("name").get_to(score.name);
        j.at("score").get_to(score.score);
        j.at("events").get_to(score.events);
    }
}

#endif
