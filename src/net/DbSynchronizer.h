#ifndef TRIPPIN_SCORESYNCHRONIZER_H
#define TRIPPIN_SCORESYNCHRONIZER_H

#include "SDL.h"
#include "Transport.h"
#include "StagingArea.h"

namespace trippin {
    class DbSynchronizer {
    public:
        DbSynchronizer(const Transport &transport, StagingArea &stagingArea);
        void runAddScoreLoop();
        void runQueryScoresLoop();
        void start();
    private:
        const Transport &transport;
        StagingArea &stagingArea;
        void addScore(const Score &score) const;
    };
}

#endif
