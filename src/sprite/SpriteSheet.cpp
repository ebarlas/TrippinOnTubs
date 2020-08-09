#include "SDL_image.h"
#include "SpriteException.h"
#include "Scale.h"
#include "SpriteSheet.h"
#include "SpriteFiles.h"

trippin::SpriteSheet::SpriteSheet(SDL_Renderer *ren, const std::string &name, Scale scale) : renderer(ren) {
    auto surface = loadImage(getSpriteSheetFile(name, scale).c_str());
    size = {surface->w, surface->h};
    texture = createTexture(renderer, surface);
    SDL_FreeSurface(surface);
}

trippin::SpriteSheet::~SpriteSheet() {
    SDL_DestroyTexture(texture);
}

SDL_Surface *trippin::SpriteSheet::loadImage(const char *path) {
    auto surface = IMG_Load(path);
    if (surface == nullptr) {
        SpriteException::throwSdlImageError(std::string{"Unable to load image "} + path + ". SDL_image Error: ");
    }
    return surface;
}

SDL_Texture *trippin::SpriteSheet::createTexture(SDL_Renderer *renderer, SDL_Surface *surface) {
    auto texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == nullptr) {
        SpriteException::throwSdlError("Unable to create texture from image. SDL Error: ");
    }
    return texture;
}

void trippin::SpriteSheet::render(SDL_Rect *clip, SDL_Rect *target) const {
    SDL_RenderCopyEx(renderer, texture, clip, target, 0, nullptr, SDL_FLIP_NONE);
}

trippin::Point<int> trippin::SpriteSheet::getSize() const {
    return size;
}
