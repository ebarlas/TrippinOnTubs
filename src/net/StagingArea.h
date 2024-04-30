#ifndef TRIPPIN_STAGINGAREA_H
#define TRIPPIN_STAGINGAREA_H

#include <string>
#include <vector>
#include <mutex>
#include "Score.h"
#include "LogEvent.h"
#include "Transport.h"

namespace trippin {
    class StagingArea {
    public:
        explicit StagingArea(Transport& transport);
        void addScore(const Score &score);
        std::vector<Score> getTodayScores(int limit) const;
        std::vector<Score> getTopScores(int limit) const;
        bool bothSet() const;
        void start();
    private:
        Transport& transport;
        mutable std::mutex mutex;
        std::vector<Score> addedScores;
        std::vector<Score> todayScores;
        std::vector<Score> topScores;
        bool todaySet{};
        bool topSet{};
        static std::vector<Score> combine(
                const std::vector<Score> &sorted,
                const std::vector<Score> &unsorted,
                int limit);
        void run();
        void setTodayScores(std::vector<Score> scores);
        void setTopScores(std::vector<Score> scores);
    };
}

#endif
