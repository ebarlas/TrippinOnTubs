#ifndef TRIPPIN_SPRITE_H
#define TRIPPIN_SPRITE_H

#include "SDL.h"
#include "sprite/SpriteMetadata.h"
#include "sprite/SpriteSheet.h"

namespace trippin {
    class Sprite {
    public:
        void load(SDL_Renderer *renderer, const std::string &name, Scale scale);
        void render(SDL_Renderer *renderer, const Point<int> position, int frame) const;
        const Point<int> &getSize() const;
        const Rect<int> &getHitBox() const;
        int getDuration() const;
        int getFrames() const;
    private:
        Point<int> size;
        Rect<int> hitBox;
        SpriteMetadata metadata{};
        SpriteSheet sheet{};
    };
}

#endif
