#include "SpriteLoadTask.h"

trippin::SpriteLoadTask::SpriteLoadTask(const SpriteLoader &spriteLoader, std::vector<std::string> names)
        : spriteLoader(spriteLoader), names(std::move(names)) {
}

trippin::SpriteLoadTask::~SpriteLoadTask() {
    if (started() && !joined()) {
        join();
    }
}

static int run(void *data) {
    auto task = (trippin::SpriteLoadTask *) data;
    task->load();
    return 0;
}

void trippin::SpriteLoadTask::start() {
    thread = SDL_CreateThread(run, "Surface Loader Thread", (void *) this);
}

bool trippin::SpriteLoadTask::started() const {
    return thread != nullptr;
}

bool trippin::SpriteLoadTask::joined() const {
    return threadJoined;
}

void trippin::SpriteLoadTask::join() {
    int status;
    SDL_WaitThread(thread, &status);
    threadJoined = true;
}

void trippin::SpriteLoadTask::load() {
    surfaces = std::make_unique<std::unordered_map<std::string, SDL_Surface *>>(loadSurfaces());
}

std::unique_ptr<std::unordered_map<std::string, SDL_Surface *>> trippin::SpriteLoadTask::take() {
    return std::move(surfaces);
}

std::unordered_map<std::string, SDL_Surface *> trippin::SpriteLoadTask::loadSurfaces() const {
    std::unordered_map<std::string, SDL_Surface *> map;
    for (auto &name : names) {
        map[name] = spriteLoader.loadSurface(name);
    }
    return map;
}