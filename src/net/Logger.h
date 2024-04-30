#ifndef TRIPPIN_LOGGER_H
#define TRIPPIN_LOGGER_H

#include <string>
#include "LogEvent.h"

namespace trippin {
    class Logger {
    public:
        Logger(std::function<void(const LogEvent&)> logFn, std::string appId);
        void log(const std::string &message);
    private:
        std::function<void(const LogEvent&)> logFn;
        const std::string appId;
        int counter;
    };
}

#endif
