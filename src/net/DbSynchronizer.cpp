#include "DbSynchronizer.h"

#include <utility>

struct Context {
    trippin::Transport transport;
    std::weak_ptr<trippin::StagingArea> stagingArea;
};

template<class T, class TakeFn, class AddFn>
static void runSyncLoop(void *data, TakeFn takeFn, AddFn addFn) {
    auto ctx = static_cast<Context *> (data);
    while (!ctx->stagingArea.expired()) {
        std::vector<T> items;
        if (auto sa = ctx->stagingArea.lock()) {
            items = takeFn(sa);
        }
        for (auto &item: items) {
            addFn(ctx->transport, item);
        }
        SDL_Delay(5'000);
    }
    delete ctx;
}

static int runAddScoresLoop(void *data) {
    auto takeFn = [](std::shared_ptr<trippin::StagingArea> &sa) {
        auto scores = sa->takeAddedScores();
        SDL_Log("took score from staging area, count=%lu", scores.size());
        return scores;
    };
    auto addFn = [](const trippin::Transport &t, const trippin::Score &s) {
        t.addScore(s);
        SDL_Log("added score, name=%s, score=%d, id=%d", s.name.c_str(), s.score, s.id);
    };
    runSyncLoop<trippin::Score>(data, takeFn, addFn);
    return 0;
}

static int runAddLogEventsLoop(void *data) {
    auto takeFn = [](std::shared_ptr<trippin::StagingArea> &sa) {
        auto events = sa->takeAddedLogEvents();
        SDL_Log("took log events from staging area, count=%lu", events.size());
        return events;
    };
    auto addFn = [](const trippin::Transport &t, const trippin::LogEvent &e) {
        t.addLogEvent(e);
        SDL_Log("added log events, message=%s", e.message.c_str());
    };
    runSyncLoop<trippin::LogEvent>(data, takeFn, addFn);
    return 0;
}

void trippin::DbSynchronizer::startAddScoresThread(Transport transport, std::weak_ptr<StagingArea> stagingArea) {
    auto ctx = new Context{std::move(transport), std::move(stagingArea)};
    SDL_DetachThread(SDL_CreateThread(runAddScoresLoop, "Add Scores Thread", (void *) ctx));
}

void trippin::DbSynchronizer::startAddLogEventsThread(Transport transport, std::weak_ptr<StagingArea> stagingArea) {
    auto ctx = new Context{std::move(transport), std::move(stagingArea)};
    SDL_DetachThread(SDL_CreateThread(runAddLogEventsLoop, "Add Log Events Thread", (void *) ctx));
}

static int runQueryScoresLoop(void *data) {
    SDL_Log("entering query scores loop");

    auto ctx = static_cast<Context *> (data);
    while (!ctx->stagingArea.expired()) {
        auto top = ctx->transport.topScores();
        if (top.ok) {
            if (auto sa = ctx->stagingArea.lock()) {
                sa->setTopScores(top.scores);
            }
            SDL_Log("set top scores in staging area, count=%lu", top.scores.size());
        }

        auto today = ctx->transport.todayScores();
        if (today.ok) {
            if (auto sa = ctx->stagingArea.lock()) {
                sa->setTodayScores(today.scores);
            }
            SDL_Log("set today scores in staging area, count=%lu", today.scores.size());
        }

        SDL_Delay(60'000);
    }

    delete ctx;

    SDL_Log("exiting query scores loop");
    return 0;
}

void trippin::DbSynchronizer::startQueryScoresThread(Transport transport, std::weak_ptr<StagingArea> stagingArea) {
    auto ctx = new Context{std::move(transport), std::move(stagingArea)};
    SDL_DetachThread(SDL_CreateThread(runQueryScoresLoop, "Query Scores Thread", (void *) ctx));
}
