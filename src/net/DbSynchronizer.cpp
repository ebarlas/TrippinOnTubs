#include "DbSynchronizer.h"

trippin::DbSynchronizer::DbSynchronizer(const Transport &transport, StagingArea &stagingArea)
        : transport(transport), stagingArea(stagingArea) {

}

int runAddLoop(void *data) {
    auto engine = (trippin::DbSynchronizer *) data;
    engine->runAddScoreLoop();
    return 0;
}

int runQueryLoop(void *data) {
    auto engine = (trippin::DbSynchronizer *) data;
    engine->runQueryScoresLoop();
    return 0;
}

void trippin::DbSynchronizer::start() {
    SDL_DetachThread(SDL_CreateThread(runAddLoop, "Add Scores Thread", (void *) this));
    SDL_DetachThread(SDL_CreateThread(runQueryLoop, "Query Scores Thread", (void *) this));
    SDL_Log("started db synchronizer threads");
}

void trippin::DbSynchronizer::runAddScoreLoop() {
    while (true) {
        auto scores = stagingArea.takeAddedScores();
        SDL_Log("took scores from stage area, count=%lu", scores.size());
        for (auto &score : scores) {
            addScore(score);
            SDL_Log("added score, name=%s, score=%d, id=%s", score.name.c_str(), score.score, score.id.c_str());
        }

        SDL_Delay(5'000);
    }
}

void trippin::DbSynchronizer::addScore(const trippin::Score &score) const {
    while (!transport.addScore(score)) {
        SDL_Log("failed to add score, sleeping for 10s");
        SDL_Delay(10'000);
    }
}

void trippin::DbSynchronizer::runQueryScoresLoop() {
    while (true) {
        auto top = transport.topScores();
        if (top.ok) {
            stagingArea.setTopScores(top.scores);
            SDL_Log("set top scores in staging area, count=%lu", top.scores.size());
        }

        auto today = transport.todayScores();
        if (today.ok) {
            stagingArea.setTodayScores(today.scores);
            SDL_Log("set today scores in staging area, count=%lu", today.scores.size());
        }

        SDL_Delay(60'000);
    }
}
