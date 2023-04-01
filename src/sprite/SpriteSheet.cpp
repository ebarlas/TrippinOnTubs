#include "SDL_image.h"
#include "Scale.h"
#include "SpriteSheet.h"

trippin::SpriteSheet::SpriteSheet(SDL_Renderer *ren, const std::string &name, const SpriteLoader &spriteLoader, int frames)
        : SpriteSheet(ren, spriteLoader.loadSurfaces(name, frames)) {
}

trippin::SpriteSheet::SpriteSheet(SDL_Renderer *renderer, const std::vector<SDL_Surface *> &surfaces) : renderer(renderer) {
    size = {surfaces[0]->w, surfaces[0]->h};
    textures.reserve(surfaces.size());
    for (int i = 0; i < surfaces.size(); i++) {
        textures[i] = SpriteLoader::createTexture(renderer, surfaces[i]);
        SDL_FreeSurface(surfaces[i]);
    }
}

trippin::SpriteSheet::~SpriteSheet() {
    for (auto texture : textures) {
        SDL_DestroyTexture(texture);
    }
}

void trippin::SpriteSheet::render(int frame, SDL_Rect *target) const {
    SDL_Rect src{0, 0, size.x, size.y};
    SDL_RenderCopyEx(renderer, textures[frame], &src, target, 0, nullptr, SDL_FLIP_NONE);
}

trippin::Point<int> trippin::SpriteSheet::getSize() const {
    return size;
}