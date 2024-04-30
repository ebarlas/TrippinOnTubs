#include "Logger.h"
#include "SDL.h"

trippin::Logger::Logger(std::function<void(const LogEvent&)> logFn, std::string appId) :
        logFn(logFn),
        appId(std::move(appId)),
        counter(0) {
}

void trippin::Logger::log(const std::string &message) {
    auto cnt = ++counter;
    logFn({appId, cnt, message});
    SDL_Log("count=%d, %s", cnt, message.c_str());
}