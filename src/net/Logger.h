#ifndef TRIPPIN_LOGGER_H
#define TRIPPIN_LOGGER_H

#include <string>
#include <ctime>
#include "SDL.h"
#include "net/StagingArea.h"

namespace trippin {
    class Logger {
    public:
        Logger(StagingArea &stagingArea);
        void log(const std::string &message);
    private:
        time_t time;
        int id;
        int counter;
        StagingArea &stagingArea;
    };
}

#endif
