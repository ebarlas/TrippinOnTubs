#ifndef TRIPPIN_SPRITESHEET_H
#define TRIPPIN_SPRITESHEET_H

#include "SDL.h"
#include "engine/Point.h"
#include "sprite/Scale.h"

namespace trippin {
    class SpriteSheet {
    public:
        SpriteSheet(SDL_Renderer *ren, const std::string &name, Scale scale);
        ~SpriteSheet();
        void render(SDL_Rect *clip, SDL_Rect *target) const;
        Point<int> getSize() const;
    private:
        Point<int> size{};
        SDL_Renderer *renderer{};
        SDL_Texture *texture{};
        static SDL_Surface *loadImage(const char *path);
        static SDL_Texture *createTexture(SDL_Renderer *renderer, SDL_Surface *surface);
    };
}

#endif
