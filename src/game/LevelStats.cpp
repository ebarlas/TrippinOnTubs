#include "LevelStats.h"

void trippin::LevelStats::onEvent(Event event, int ticks) {
    lastTimes[event] = ticks;
    counts[event]++;
}

int trippin::LevelStats::lastTime(Event event) const {
    return getWithFallback(lastTimes, event, -1);
}

int trippin::LevelStats::count(Event event) const {
    return getWithFallback(counts, event, -1);
}

int trippin::LevelStats::getWithFallback(const std::unordered_map<Event, int> &m, Event key, int fallback) {
    auto it = m.find(key);
    if (it != m.end()) {
        return it->second;
    }
    return fallback;
}
