#ifndef TRIPPIN_FONTRENDERER_H
#define TRIPPIN_FONTRENDERER_H

#include <string>
#include "SDL.h"
#include "SDL_ttf.h"
#include "engine/Point.h"

namespace trippin {
    class FontRenderer {
    private:
        SDL_Renderer *renderer;
        TTF_Font *font;
    public:
        FontRenderer(SDL_Renderer *renderer);
        ~FontRenderer();
        void render(const std::string &text, const SDL_Color &color, const trippin::Point<int> &pos);
    };
}

#endif
