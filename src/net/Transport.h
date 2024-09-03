#ifndef TRIPPIN_TRANSPORT_H
#define TRIPPIN_TRANSPORT_H

#include <string>
#include <vector>
#include "Score.h"
#include "LogEvent.h"
#include "AddResult.h"

namespace trippin {
    class Transport {
    public:
        struct Scores {
            std::vector<Score> scores;
            bool ok{};
        };

        Transport(std::string host, int port, int major, int minor, int limit);
        [[nodiscard]] AddResult addScore(const Score &score) const;
        [[nodiscard]] AddResult addLogEvent(const LogEvent &event) const;
        [[nodiscard]] Scores topScores() const;
        [[nodiscard]] Scores todayScores() const;
        [[nodiscard]] std::string getNotification();
    private:
        const std::string host;
        const int port;
        const int major;
        const int minor;
        const int limit;
        static AddResult classifyResponse(const std::string &response);
        [[nodiscard]] Scores sendRequest(const std::string &uri) const;
        static bool validNotification(std::string &notification);
        static bool validNotificationChar(char c);
    };
}

#endif
