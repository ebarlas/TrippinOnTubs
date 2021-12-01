#ifndef TRIPPIN_SPRITELOADTASK_H
#define TRIPPIN_SPRITELOADTASK_H

#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <thread>
#include "SDL.h"
#include "Scale.h"
#include "SpriteLoader.h"

namespace trippin {
    // A background task for loading surfaces into an unordered map for a predetermined set of sprites.
    // The call sequence from the main thread is: (1) start -> (2) join -> (3) take
    class SpriteLoadTask {
    public:
        SpriteLoadTask(const SpriteLoader &spriteLoader, std::vector<std::string> names);
        ~SpriteLoadTask();
        void start();
        void join();
        void load();
        bool joined() const;
        bool started() const;
        std::unique_ptr<std::unordered_map<std::string, SDL_Surface *>> take();
    private:
        std::unique_ptr<std::unordered_map<std::string, SDL_Surface *>> surfaces;
        std::vector<std::string> names;
        std::thread thread;
        const SpriteLoader &spriteLoader;
        bool threadStarted{};
        bool threadJoined{};

        std::unordered_map<std::string, SDL_Surface *> loadSurfaces() const;
    };
}

#endif
