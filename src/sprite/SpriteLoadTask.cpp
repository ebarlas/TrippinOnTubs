#include "SpriteLoadTask.h"
#include "SpriteMetadata.h"

trippin::SpriteLoadTask::SpriteLoadTask(const SpriteLoader &spriteLoader, std::vector<std::string> names)
        : names(std::move(names)), spriteLoader(spriteLoader) {
}

trippin::SpriteLoadTask::~SpriteLoadTask() {
    if (started() && !joined()) {
        join();
    }
}

static void run(trippin::SpriteLoadTask *task) {
    task->load();
}

void trippin::SpriteLoadTask::start() {
    thread = std::thread(run, this);
    threadStarted = true;
}

bool trippin::SpriteLoadTask::started() const {
    return threadStarted;
}

bool trippin::SpriteLoadTask::joined() const {
    return threadJoined;
}

void trippin::SpriteLoadTask::join() {
    thread.join();
    threadJoined = true;
}

void trippin::SpriteLoadTask::load() {
    SDL_Log("loading surfaces, count=%lu", names.size());
    surfaces = std::make_unique<std::unordered_map<std::string, std::vector<SDL_Surface *>>>(loadSurfaces());
    SDL_Log("done loading surfaces");
}

std::unique_ptr<std::unordered_map<std::string, std::vector<SDL_Surface *>>> trippin::SpriteLoadTask::take() {
    return std::move(surfaces);
}

std::unordered_map<std::string, std::vector<SDL_Surface *>> trippin::SpriteLoadTask::loadSurfaces() const {
    std::unordered_map<std::string, std::vector<SDL_Surface *>> map;
    for (auto &name: names) {
        SpriteMetadata sm;
        sm.load(name);
        map[name] = spriteLoader.loadSurfaces(name, sm.getFrames());
    }
    return map;
}