#ifndef TRIPPIN_SPRITEMANAGER_H
#define TRIPPIN_SPRITEMANAGER_H

#include <unordered_map>
#include <string>
#include "sprite/Sprite.h"
#include "sprite/SpriteType.h"

namespace trippin {
    class SpriteManager {
    public:
        SpriteManager(SDL_Renderer *renderer, Scale scale, int tickPeriod);
        const Sprite &get(const std::string &type);
    private:
        using SpritePtr = std::unique_ptr<Sprite>;
        const Scale scale;
        const int tickPeriod;
        SDL_Renderer *renderer;
        std::unordered_map<std::string, SpritePtr> sprites;
    };
}

#endif
