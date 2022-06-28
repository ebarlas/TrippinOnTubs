#include "Logger.h"
#include "SDL.h"

trippin::Logger::Logger(StagingArea &stagingArea, std::string appId) :
        stagingArea(stagingArea),
        appId(std::move(appId)),
        counter(0) {
}

void trippin::Logger::log(const std::string &message) {
    auto cnt = ++counter;
    stagingArea.addLogEvent({appId, cnt, message});
    SDL_Log("count=%d, %s", cnt, message.c_str());
}