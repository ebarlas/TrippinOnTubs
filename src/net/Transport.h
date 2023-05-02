#ifndef TRIPPIN_TRANSPORT_H
#define TRIPPIN_TRANSPORT_H

#include <string>
#include <string_view>
#include <vector>
#include "Score.h"
#include "LogEvent.h"
#include "nlohmann/json.hpp"

namespace trippin {
    class Transport {
    public:
        struct Scores {
            std::vector<Score> scores;
            bool ok{};
        };

        Transport(std::string host, int port, int version, int limit);
        bool addScore(const Score &score) const;
        bool addLogEvent(const LogEvent &event) const;
        Scores topScores() const;
        Scores todayScores() const;
    private:
        const std::string host;
        const int port;
        const int version;
        const int limit;
        static std::vector<int> compress(const std::vector<Score::InputEvent> &events);
        static std::vector<std::vector<int>> compress(const std::vector<std::vector<Score::InputEvent>> &vecs);
        Scores sendRequest(const std::string &uri) const;
    };

    void from_json(const nlohmann::json &j, Score &score);
    void from_json(const nlohmann::json &j, std::vector<Score::InputEvent> &points);
}

#endif
