#include <algorithm>
#include "StagingArea.h"

void trippin::StagingArea::addScore(const Score &score) {
    std::lock_guard<std::mutex> lock(mutex);
    addedScores.push_back(score);
    outgoingScores.push_back(score);
}

void trippin::StagingArea::addLogEvent(const LogEvent &logEvent) {
    std::lock_guard<std::mutex> lock(mutex);
    outgoingLogEvents.push_back(logEvent);
}

void trippin::StagingArea::setTodayScores(std::vector<Score> scores) {
    std::lock_guard<std::mutex> lock(mutex);
    todayScores = std::move(scores);
    todaySet = true;
}

void trippin::StagingArea::setTopScores(std::vector<Score> scores) {
    std::lock_guard<std::mutex> lock(mutex);
    topScores = std::move(scores);
    topSet = true;
}

std::vector<trippin::Score> trippin::StagingArea::getTodayScores(int limit) const {
    std::lock_guard<std::mutex> lock(mutex);
    return combine(todayScores, addedScores, limit);
}

std::vector<trippin::Score> trippin::StagingArea::getTopScores(int limit) const {
    std::lock_guard<std::mutex> lock(mutex);
    return combine(topScores, addedScores, limit);
}

std::vector<trippin::Score> trippin::StagingArea::takeAddedScores() {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<Score> scores = std::move(outgoingScores);
    outgoingScores.clear();
    return scores;
}

std::vector<trippin::LogEvent> trippin::StagingArea::takeAddedLogEvents() {
    std::lock_guard<std::mutex> lock(mutex);
    std::vector<LogEvent> events = std::move(outgoingLogEvents);
    outgoingLogEvents.clear();
    return events;
}

std::vector<trippin::Score> trippin::StagingArea::combine(
        const std::vector<Score> &sorted,
        const std::vector<Score> &unsorted,
        int limit) {
    std::vector<Score> scores(sorted);
    auto fn = [](const Score &left, const Score &right) {
        return left.score > right.score;
    };
    for (auto &sc : unsorted) {
        if (std::find(scores.begin(), scores.end(), sc) == scores.end()) {
            scores.insert(std::upper_bound(scores.begin(), scores.end(), sc, fn), sc);
        }
    }
    if (scores.size() > limit) {
        scores.resize(limit);
    }
    return scores;
}

bool trippin::StagingArea::bothSet() const {
    return topSet && todaySet;
}
