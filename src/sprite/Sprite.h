#ifndef TRIPPIN_SPRITE_H
#define TRIPPIN_SPRITE_H

#include "SDL.h"
#include "sprite/SpriteMetadata.h"
#include "sprite/SpriteSheet.h"

namespace trippin {
    class Sprite {
    public:
        void load(SDL_Renderer *renderer, const std::string &name, Scale scale);
        void render(SDL_Renderer *renderer, const Vector<int> position, int frame) const;
        Vector<int> getSize() const;
    private:
        SpriteMetadata metadata{};
        SpriteSheet sheet{};
    };
}

#endif
