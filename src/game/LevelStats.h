#ifndef TRIPPIN_LEVELSTATS_H
#define TRIPPIN_LEVELSTATS_H

#include <unordered_map>

namespace trippin {
    class LevelStats {
    public:
        enum class Event {
            Jump,
            Duck,
            Stop,
            ChargedJump,
            DuckJump,
            DoubleJump,
            JumpSlamDown,
            WingedTub
        };

        void onEvent(Event event, int ticks);

        int lastTime(Event event) const;
        int count(Event event) const;
        bool exists(Event event) const;
    private:
        std::unordered_map<Event, int> lastTimes;
        std::unordered_map<Event, int> counts;
        static int getWithFallback(const std::unordered_map<Event, int> &m, Event key, int fallback);
    };
}

#endif
