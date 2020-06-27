#ifndef TRIPPIN_FONTRENDERER_H
#define TRIPPIN_FONTRENDERER_H

#include <string>
#include "SDL.h"
#include "SDL2/SDL_ttf.h"
#include "engine/vector.h"

namespace trippin {
    class FontRenderer {
    private:
        SDL_Renderer *renderer;
        TTF_Font *font;
    public:
        FontRenderer(SDL_Renderer *renderer);
        ~FontRenderer();
        void render(const std::string &text, const SDL_Color &color, const trippin::Vector<int> &pos);
    };
}

#endif
