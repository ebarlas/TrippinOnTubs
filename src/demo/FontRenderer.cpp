#include "FontRenderer.h"

trippin::FontRenderer::FontRenderer(SDL_Renderer *renderer) : renderer(renderer), font(TTF_OpenFont("fonts/lazy.ttf", 16)) {
}

void trippin::FontRenderer::render(const std::string &text, const SDL_Color &color, const trippin::Point<int> &pos) {
    auto surface = TTF_RenderText_Solid(font, text.c_str(), color);
    auto texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect clip{pos.x, pos.y, surface->w, surface->h};
    SDL_RenderCopyEx(renderer, texture, nullptr, &clip, 0, nullptr, SDL_FLIP_NONE);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

trippin::FontRenderer::~FontRenderer() {
    TTF_CloseFont(font);
}
