#ifndef TRIPPIN_SPRITEMANAGER_H
#define TRIPPIN_SPRITEMANAGER_H

#include <unordered_map>
#include <string>
#include "engine/Fraction.h"
#include "sprite/Sprite.h"
#include "SpriteLoadTask.h"

namespace trippin {
    class SpriteManager {
    public:
        SpriteManager(SDL_Renderer *renderer, SpriteLoader &spriteLoader, const Units &units, Fraction<int> tickPeriod);
        const Sprite &get(const std::string &type);
        void setSurfaces(std::unique_ptr<std::unordered_map<std::string, SDL_Surface *>> surfaces);
    private:
        using SpritePtr = std::unique_ptr<Sprite>;
        SDL_Renderer *renderer;
        SpriteLoader &spriteLoader;
        const Units &units;
        const Fraction<int> tickPeriod;
        std::unordered_map<std::string, SpritePtr> sprites;
        std::unique_ptr<std::unordered_map<std::string, SDL_Surface *>> surfaces;

        SpritePtr newSprite(const std::string &type);
    };
}

#endif
