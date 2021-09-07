#ifndef TRIPPIN_LOG_EVENT_H
#define TRIPPIN_LOG_EVENT_H

#include <ctime>
#include <string>

namespace trippin {
    struct LogEvent {
        int id;
        time_t time;
        int index;
        std::string message;
    };
}

#endif
