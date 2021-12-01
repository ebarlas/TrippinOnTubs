#ifndef TRIPPIN_DBSYNCHRONIZER_H
#define TRIPPIN_DBSYNCHRONIZER_H

#include <memory>
#include "Transport.h"
#include "StagingArea.h"

namespace trippin {
    class DbSynchronizer {
    public:
        static void startAddScoresThread(Transport transport, std::weak_ptr<StagingArea> stagingArea);
        static void startAddLogEventsThread(Transport transport, std::weak_ptr<StagingArea> stagingArea);
        static void startQueryScoresThread(Transport transport, std::weak_ptr<StagingArea> stagingArea);
    };
}

#endif
