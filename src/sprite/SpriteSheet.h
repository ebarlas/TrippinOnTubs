#ifndef TRIPPIN_SPRITESHEET_H
#define TRIPPIN_SPRITESHEET_H

#include "SDL.h"
#include "engine/Point.h"
#include "sprite/Scale.h"
#include "SpriteLoader.h"

namespace trippin {
    class SpriteSheet {
    public:
        SpriteSheet(SDL_Renderer *ren, const std::string &name, const SpriteLoader &spriteLoader);
        SpriteSheet(SDL_Renderer *renderer, SDL_Surface *surface);
        SpriteSheet(const SpriteSheet &) = delete;
        SpriteSheet(SpriteSheet &&) = delete;
        SpriteSheet& operator=(const SpriteSheet &&) = delete;
        SpriteSheet& operator=(SpriteSheet &&) = delete;
        ~SpriteSheet();
        void render(SDL_Rect *clip, SDL_Rect *target) const;
        // Get the total size of the sprite sheet in pixels
        // Pixels are the in target sprite scale (hdplus, fdh, etc)
        Point<int> getSize() const;
    private:
        Point<int> size{};
        SDL_Renderer *renderer;
        SDL_Texture *texture;
    };
}

#endif
