#ifndef TRIPPIN_TRANSPORT_H
#define TRIPPIN_TRANSPORT_H

#include <string>
#include <vector>
#include "Score.h"
#include "LogEvent.h"

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
}

#endif
