#ifndef TRIPPIN_LOGGER_H
#define TRIPPIN_LOGGER_H

#include <string>
#include "net/StagingArea.h"

namespace trippin {
    class Logger {
    public:
        Logger(StagingArea &stagingArea, std::string appId);
        void log(const std::string &message);
    private:
        StagingArea &stagingArea;
        const std::string appId;
        int counter;
    };
}

#endif
