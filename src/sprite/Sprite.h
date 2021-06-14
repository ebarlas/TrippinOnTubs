#ifndef TRIPPIN_SPRITE_H
#define TRIPPIN_SPRITE_H

#include "SDL.h"
#include "sprite/SpriteMetadata.h"
#include "sprite/SpriteSheet.h"
#include "Camera.h"

namespace trippin {
    class Sprite {
    public:
        Sprite(SDL_Renderer *renderer, const std::string &name, const SpriteLoader &spriteLoader, int tickPeriod);
        Sprite(SDL_Renderer *renderer, const std::string &name, const Scale &scale, int tickPeriod, SDL_Surface *sur);
        void render(Point<int> position, int frame) const;
        void render(Point<int> hitBoxPos, int frame, const Camera &camera) const;
        Point<int> getSize() const;
        Rect<int> getHitBox() const;
        int getFramePeriodTicks() const;
        int getFrameDuration() const;
        int getFrames() const;
        SDL_Renderer* getRenderer() const;
        bool intersectsWith(Point<int> hitBoxPos, Rect<int> rect) const;
        const Scale& getScale() const;
    private:
        const Scale& scale;
        Point<int> size;
        Rect<int> hitBox;
        SpriteMetadata metadata;
        SpriteSheet sheet;
        // Duration of each frame in ticks
        // If frame duration is 80ms and tick period is 10ms, the frame period in ticks is 10
        int framePeriodTicks;
        SDL_Renderer *ren;

        void init(const std::string &name, int tickPeriod);
    };
}

#endif
