#ifndef TRIPPIN_SPRITESHEET_H
#define TRIPPIN_SPRITESHEET_H

#include "SDL.h"
#include "engine/Point.h"
#include "sprite/Scale.h"
#include "sdl/Texture.h"

namespace trippin {
    class SpriteSheet {
    public:
        SpriteSheet(SDL_Renderer *ren, const std::string &name, Scale scale);
        void render(SDL_Rect *clip, SDL_Rect *target) const;
        Point<int> getSize() const;
    private:
        Point<int> size{};
        SDL_Renderer *renderer{};
        std::unique_ptr<Texture> texture;
        static std::string getSpriteSheetFile(const std::string& name, Scale scale);
    };
}

#endif
