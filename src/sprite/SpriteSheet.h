#ifndef TRIPPIN_SPRITESHEET_H
#define TRIPPIN_SPRITESHEET_H

#include <vector>
#include "SDL.h"
#include "engine/Point.h"
#include "sprite/Scale.h"
#include "SpriteLoader.h"

namespace trippin {
    class SpriteSheet {
    public:
        SpriteSheet(SDL_Renderer *ren, const std::string &name, const SpriteLoader &spriteLoader, int frames);
        SpriteSheet(SDL_Renderer *renderer, const std::vector<SDL_Surface *> &surface);
        SpriteSheet(const SpriteSheet &) = delete;
        SpriteSheet(SpriteSheet &&) = delete;
        SpriteSheet& operator=(const SpriteSheet &&) = delete;
        SpriteSheet& operator=(SpriteSheet &&) = delete;
        ~SpriteSheet();
        void render(int frame, SDL_Rect *target) const;
        Point<int> getSize() const;
    private:
        Point<int> size{};
        SDL_Renderer *renderer;
        std::vector<SDL_Texture *> textures;
    };
}

#endif
