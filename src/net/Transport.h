#ifndef TRIPPIN_TRANSPORT_H
#define TRIPPIN_TRANSPORT_H

#include <string>
#include <vector>
#include "Score.h"
#include "nlohmann/json.hpp"

namespace trippin {
    class Transport {
    public:
        struct Scores {
            std::vector<Score> scores;
            bool ok{};
        };

        Transport(std::string host, int port);
        bool addScore(const Score &score) const;
        Scores topScores() const;
        Scores todayScores() const;
    private:
        std::string host;
        int port;
        Scores sendRequest(const std::string& uri) const;
    };

    void from_json(const nlohmann::json &j, Score &score);
}

#endif
