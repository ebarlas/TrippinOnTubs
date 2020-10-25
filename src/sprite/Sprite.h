#ifndef TRIPPIN_SPRITE_H
#define TRIPPIN_SPRITE_H

#include "SDL.h"
#include "sprite/SpriteMetadata.h"
#include "sprite/SpriteSheet.h"

namespace trippin {
    class Sprite {
    public:
        Sprite(SDL_Renderer *renderer, const std::string &name, const Scale &scale);
        void render(Point<int> position, int frame) const;
        Point<int> getSize() const;
        Rect<int> getHitBox() const;
        int getDuration() const;
        int getFrames() const;
        const Scale &getScale() const;
    private:
        const Scale &scale;
        Point<int> size;
        Rect<int> hitBox;
        SpriteMetadata metadata;
        SpriteSheet sheet;
    };
}

#endif
