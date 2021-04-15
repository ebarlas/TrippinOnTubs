#include "DbSynchronizer.h"

#include <utility>

struct Context {
    trippin::Transport transport;
    std::weak_ptr<trippin::StagingArea> stagingArea;
};

int runAddScoresLoop(void *data) {
    SDL_Log("entering add scores loop");

    auto ctx = static_cast<Context *> (data);
    while (!ctx->stagingArea.expired()) {
        std::vector<trippin::Score> scores;
        if (auto sa = ctx->stagingArea.lock()) {
            scores = sa->takeAddedScores();
        }

        SDL_Log("took scores from stage area, count=%lu", scores.size());

        for (auto &score : scores) {
            ctx->transport.addScore(score);
            SDL_Log("added score, name=%s, score=%d, id=%s", score.name.c_str(), score.score, score.id.c_str());
        }

        SDL_Delay(5'000);
    }

    delete ctx;

    SDL_Log("exiting add scores loop");
    return 0;
}

void trippin::DbSynchronizer::startAddScoresThread(Transport transport, std::weak_ptr<StagingArea> stagingArea) {
    auto ctx = new Context{std::move(transport), std::move(stagingArea)};
    SDL_DetachThread(SDL_CreateThread(runAddScoresLoop, "Add Scores Thread", (void *) ctx));
}

int runQueryScoresLoop(void *data) {
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
