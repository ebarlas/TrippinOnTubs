#ifndef TRIPPIN_STAGINGAREA_H
#define TRIPPIN_STAGINGAREA_H

#include <string>
#include <vector>
#include <mutex>
#include "Score.h"
#include "LogEvent.h"

namespace trippin {
    class StagingArea {
    public:
        void addScore(const Score &score);
        void addLogEvent(const LogEvent &logEvent);
        void setTodayScores(std::vector<Score> scores);
        void setTopScores(std::vector<Score> scores);
        std::vector<Score> takeAddedScores();
        std::vector<LogEvent> takeAddedLogEvents();
        std::vector<Score> getTodayScores(int limit) const;
        std::vector<Score> getTopScores(int limit) const;
        bool bothSet() const;
    private:
        mutable std::mutex mutex;
        std::vector<Score> addedScores;
        std::vector<Score> outgoingScores;
        std::vector<Score> todayScores;
        std::vector<Score> topScores;
        std::vector<LogEvent> outgoingLogEvents;
        bool todaySet{};
        bool topSet{};
        static std::vector<Score> combine(
                const std::vector<Score> &sorted,
                const std::vector<Score> &unsorted,
                int limit);
    };
}

#endif
