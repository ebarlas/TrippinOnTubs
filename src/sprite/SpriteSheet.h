#ifndef TRIPPIN_SPRITESHEET_H
#define TRIPPIN_SPRITESHEET_H

#include "SDL.h"
#include "engine/Vector.h"
#include "sprite/Scale.h"

namespace trippin {
    class SpriteSheet {
    public:
        void load(SDL_Renderer *renderer, const std::string &name, Scale scale);
        void render(SDL_Renderer *renderer, SDL_Rect *clip, SDL_Rect *target) const;
        const Vector<int> &getSize() const;
    private:
        Vector<int> size{};
        SDL_Texture *texture{};
        static SDL_Surface *loadImage(const char *path);
        static SDL_Texture *createTexture(SDL_Renderer *renderer, SDL_Surface *surface);
    };
}

#endif
