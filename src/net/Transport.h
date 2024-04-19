#ifndef TRIPPIN_TRANSPORT_H
#define TRIPPIN_TRANSPORT_H

#include <string>
#include <vector>
#include <optional>
#include "Score.h"
#include "LogEvent.h"

namespace trippin {
    class Transport {
    public:
        Transport(std::string host, int port, int version, int limit);
        std::optional<std::string> addScore(const Score &score) const;
        bool addLogEvent(const LogEvent &event) const;
        std::optional<std::vector<Score>> topScores() const;
        std::optional<std::vector<Score>> todayScores() const;
    private:
        const std::string host;
        const int port;
        const int version;
        const int limit;
        static std::vector<int> compress(const std::vector<Score::InputEvent> &events);
        static std::vector<std::vector<int>> compress(const std::vector<std::vector<Score::InputEvent>> &vecs);
        std::optional<std::vector<Score>> getScores(const std::string &uri) const;
        static nlohmann::json parseResponse(std::string &response);
    };
}

#endif
