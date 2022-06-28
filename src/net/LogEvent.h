#ifndef TRIPPIN_LOG_EVENT_H
#define TRIPPIN_LOG_EVENT_H

#include <ctime>
#include <string>

namespace trippin {
    struct LogEvent {
        std::string id;
        int index;
        std::string message;
    };
}

#endif
