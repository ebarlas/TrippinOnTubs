#include <algorithm>
#include <thread>
#include "SDL.h"
#include "StagingArea.h"

trippin::StagingArea::StagingArea(trippin::Transport &transport) : transport(transport) {}

void trippin::StagingArea::addScore(const Score &score) {
    std::lock_guard<std::mutex> lock(mutex);
    addedScores.push_back(score);
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

void trippin::StagingArea::start() {
    auto in = std::thread(&StagingArea::run, this);
    in.detach();
}

void trippin::StagingArea::run() {
    SDL_Log("entering query scores loop");
    while (true) {
        auto top = transport.topScores();
        if (top.ok) {
            setTopScores(top.scores);
            SDL_Log("set top scores in staging area, count=%lu", top.scores.size());
        }
        auto today = transport.todayScores();
        if (today.ok) {
            setTodayScores(today.scores);
            SDL_Log("set today scores in staging area, count=%lu", today.scores.size());
        }
        std::this_thread::sleep_for(std::chrono::minutes(1));
    }
}
