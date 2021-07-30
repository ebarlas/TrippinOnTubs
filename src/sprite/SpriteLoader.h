#ifndef TRIPPIN_SPRITELOADER_H
#define TRIPPIN_SPRITELOADER_H

#include <string>
#include "SDL.h"
#include "Scale.h"

namespace trippin {
    // Factory for sprite surfaces given at a particular scale
    // Surfaces are loaded individually with load function
    class SpriteLoader {
    public:
        SpriteLoader(Scale scale);
        SDL_Surface *loadSurface(const std::string &name) const;
        static SDL_Texture *createTexture(SDL_Renderer *renderer, SDL_Surface *surface);
        const Scale& getScale() const;
    private:
        const Scale scale;
        static SDL_Surface *loadSurface(const char *path);
        static std::string getSpriteSheetFile(const std::string &name, const Scale &scale);
    };
}

#endif
