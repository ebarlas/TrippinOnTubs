#ifndef TRIPPIN_SPRITEMANAGER_H
#define TRIPPIN_SPRITEMANAGER_H

#include <unordered_map>
#include <string>
#include "sprite/Sprite.h"
#include "sprite/SpriteType.h"

namespace trippin {
    class SpriteManager {
    public:
        void setScale(Scale scale);
        void load(SDL_Renderer *renderer);
        const Sprite& get(SpriteType type);
    private:
        using SpritePtr = std::unique_ptr<trippin::Sprite>;
        Scale scale{};
        std::unordered_map<SpriteType, SpritePtr> sprites;
    };
}

#endif
