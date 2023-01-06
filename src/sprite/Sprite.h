#ifndef TRIPPIN_SPRITE_H
#define TRIPPIN_SPRITE_H

#include "SDL.h"
#include "sprite/SpriteMetadata.h"
#include "sprite/SpriteSheet.h"
#include "Camera.h"

namespace trippin {
    class Sprite {
    public:
        Sprite(SDL_Renderer *renderer, const std::string &name, const SpriteLoader &spriteLoader, double tickPeriod);
        Sprite(SDL_Renderer *renderer, const std::string &name, const Scale &scale, double  tickPeriod, SDL_Surface *sur);
        void renderDevice(Point<int> position, int frame) const;
        void renderEngine(Point<int> hitBoxPos, int frame, const Rect<int> &viewport) const;
        Point<int> getEngineSize() const;
        Point<int> getDeviceSize() const;
        Rect<int> getEngineHitBox() const;
        int getFramePeriodTicks() const;
        int getFrameDuration() const;
        int getFrames() const;
        SDL_Renderer* getRenderer() const;
        const Scale& getScale() const;
        void advanceFrame(int ticks, int &frame, bool flash = false) const;
    private:
        const Scale& scale;
        Point<int> engineSize;
        Point<int> deviceSize;
        Rect<int> engineHitBox;
        SpriteMetadata metadata;
        SpriteSheet sheet;
        // Duration of each frame in ticks
        // If frame duration is 80ms and tick period is 10ms, the frame period in ticks is 8
        int framePeriodTicks;
        SDL_Renderer *ren;

        void init(const std::string &name, double tickPeriod);
    };
}

#endif
