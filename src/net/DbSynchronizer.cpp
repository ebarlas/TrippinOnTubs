#include "DbSynchronizer.h"

#include <utility>
#include <vector>
#include <thread>
#include "SDL.h"

using namespace trippin;

template<class T, class TakeFn, class AddFn>
static void runSyncLoop(Transport transport, std::weak_ptr<StagingArea> stagingArea, TakeFn takeFn, AddFn addFn) {
    while (!stagingArea.expired()) {
        std::vector<T> items;
        if (auto sa = stagingArea.lock()) {
            items = takeFn(sa);
        }
        for (auto &item: items) {
            addFn(transport, item);
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

static void runAddScoresLoop(Transport transport, std::weak_ptr<StagingArea> stagingArea) {
    auto takeFn = [](std::shared_ptr<StagingArea> &sa) {
        auto scores = sa->takeAddedScores();
        SDL_Log("took score from staging area, count=%lu", scores.size());
        return scores;
    };
    auto addFn = [](const Transport &t, const Score &s) {
        auto success = t.addScore(s);
        if (success) {
            SDL_Log("added score, name=%s, score=%d, id=%d", s.name.c_str(), s.score, s.id);
        } else {
            SDL_Log("failed to add score, name=%s, score=%d, id=%d", s.name.c_str(), s.score, s.id);
        }
    };
    runSyncLoop<Score>(std::move(transport), std::move(stagingArea), takeFn, addFn);
}

static void runAddLogEventsLoop(Transport transport, std::weak_ptr<StagingArea> stagingArea) {
    auto takeFn = [](std::shared_ptr<StagingArea> &sa) {
        auto events = sa->takeAddedLogEvents();
        SDL_Log("took log events from staging area, count=%lu", events.size());
        return events;
    };
    auto addFn = [](const Transport &t, const LogEvent &e) {
        auto success = t.addLogEvent(e);
        if (success) {
            SDL_Log("added log events, message=%s", e.message.c_str());
        } else {
            SDL_Log("failed to add log events, message=%s", e.message.c_str());
        }
    };
    runSyncLoop<LogEvent>(std::move(transport), std::move(stagingArea), takeFn, addFn);
}

void DbSynchronizer::startAddScoresThread(Transport transport, std::weak_ptr<StagingArea> stagingArea) {
    auto t = std::thread(runAddScoresLoop, std::move(transport), std::move(stagingArea));
    t.detach();
}

void DbSynchronizer::startAddLogEventsThread(Transport transport, std::weak_ptr<StagingArea> stagingArea) {
    auto t = std::thread(runAddLogEventsLoop, std::move(transport), std::move(stagingArea));
    t.detach();
}

static int runQueryScoresLoop(Transport transport, std::weak_ptr<StagingArea> stagingArea) {
    SDL_Log("entering query scores loop");

    while (!stagingArea.expired()) {
        auto top = transport.topScores();
        if (top.ok) {
            if (auto sa = stagingArea.lock()) {
                sa->setTopScores(top.scores);
            }
            SDL_Log("set top scores in staging area, count=%lu", top.scores.size());
        }

        auto today = transport.todayScores();
        if (today.ok) {
            if (auto sa = stagingArea.lock()) {
                sa->setTodayScores(today.scores);
            }
            SDL_Log("set today scores in staging area, count=%lu", today.scores.size());
        }

        std::this_thread::sleep_for(std::chrono::minutes(1));
    }

    SDL_Log("exiting query scores loop");
    return 0;
}

void DbSynchronizer::startQueryScoresThread(Transport transport, std::weak_ptr<StagingArea> stagingArea) {
    auto t = std::thread(runQueryScoresLoop, std::move(transport), std::move(stagingArea));
    t.detach();
}
