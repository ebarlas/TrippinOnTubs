#include "Logger.h"

trippin::Logger::Logger(StagingArea &stagingArea) : stagingArea(stagingArea) {
    time = std::time(0);
    id = std::rand();
    counter = 0;
    SDL_Log("initialized logger, time=%ld, id=%d", time, id);
}

void trippin::Logger::log(const std::string &message) {
    auto cnt = ++counter;
    stagingArea.addLogEvent({id, time, cnt, message});
    SDL_Log("count=%d, %s", cnt, message.c_str());
}