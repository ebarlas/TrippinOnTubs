#include "SDL_image.h"
#include "Scale.h"
#include "SpriteSheet.h"

trippin::SpriteSheet::SpriteSheet(SDL_Renderer *ren, const std::string &name, const SpriteLoader &spriteLoader)
        : SpriteSheet(ren, spriteLoader.loadSurface(name)) {
}

trippin::SpriteSheet::SpriteSheet(SDL_Renderer *renderer, SDL_Surface *surface) : renderer(renderer) {
    size = {surface->w, surface->h};
    texture = SpriteLoader::createTexture(renderer, surface);
    SDL_FreeSurface(surface);
}

trippin::SpriteSheet::~SpriteSheet() {
    SDL_DestroyTexture(texture);
}

void trippin::SpriteSheet::render(SDL_Rect *clip, SDL_Rect *target) const {
    SDL_RenderCopyEx(renderer, texture, clip, target, 0, nullptr, SDL_FLIP_NONE);
}

trippin::Point<int> trippin::SpriteSheet::getSize() const {
    return size;
}