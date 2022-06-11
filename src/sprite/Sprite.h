#ifndef TRIPPIN_SPRITE_H
#define TRIPPIN_SPRITE_H

#include "SDL.h"
#include "engine/Fraction.h"
#include "sprite/SpriteMetadata.h"
#include "sprite/SpriteSheet.h"
#include "sprite/Camera.h"
#include "sprite/Units.h"

namespace trippin {
    class Sprite {
    public:
        Sprite(
                SDL_Renderer *renderer,
                const std::string &name,
                const Units &units,
                Fraction<int> tickPeriod,
                const SpriteLoader &spriteLoader);
        Sprite(
                SDL_Renderer *renderer,
                const std::string &name,
                const Units &units,
                Fraction<int> tickPeriod,
                SDL_Surface *sur);
        // Render a frame in the target sprite scale at the specified absolute window position
        // The position is in pixels within the native window
        void render(Point<int> position, int frame) const;
        // Render a frame in the target sprite scale at the specified hit box position
        // The hit box position is in game engine distance units
        void render(Point<int_fast64_t> hitBoxPos, int frame, const Camera &camera) const;
        // Get the size of a single frame
        // Pixels are the in target sprite scale (hdplus, fdh, etc)
        Point<int> getSize() const;
        // Get the hit box rectangle from metadata file, a relative area within the larger sprite rectangle
        // Pixels are the in target sprite scale
        Rect<int> getHitBox() const;
        int getFramePeriodTicks() const;
        int getFrames() const;
        SDL_Renderer *getRenderer() const;
    private:
        SDL_Renderer *ren;
        const Units &units;
        Point<int> size;
        Rect<int> hitBox;
        SpriteMetadata metadata;
        SpriteSheet sheet;
        // Duration of each frame in ticks
        // If frame duration is 80ms and tick period is 10ms, the frame period in ticks is 8
        int framePeriodTicks;

        void init(const std::string &name, Fraction<int> tickPeriod);
    };
}

#endif
