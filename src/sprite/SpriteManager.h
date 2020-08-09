#ifndef TRIPPIN_SPRITEMANAGER_H
#define TRIPPIN_SPRITEMANAGER_H

#include <unordered_map>
#include <string>
#include <array>
#include "sprite/Sprite.h"
#include "sprite/SpriteType.h"

namespace trippin {
    class SpriteManager {
    public:
        SpriteManager(SDL_Renderer *renderer, Scale scale);
        const Sprite& get(SpriteType type);
    private:
        using SpritePtr = std::unique_ptr<Sprite>;
        Scale scale{};
        std::array<SpritePtr, numSprites> sprites;
    };
}

#endif
