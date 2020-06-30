#include <sstream>
#include "SDL_image.h"
#include "Scale.h"
#include "SpriteSheet.h"
#include "exception/TubsException.h"

SDL_Surface *trippin::SpriteSheet::loadImage(const char *path) {
    auto surface = IMG_Load(path);
    if (surface == nullptr) {
        std::stringstream message;
        message << "Unable to load image " << path << ". SDL_image Error: " << IMG_GetError();
        throw TubsException{message.str()};
    }
    return surface;
}

SDL_Texture *trippin::SpriteSheet::createTexture(SDL_Renderer *renderer, SDL_Surface *surface) {
    auto texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == nullptr) {
        std::stringstream message;
        message << "Unable to create texture from image. SDL Error: " << SDL_GetError();
        throw TubsException{message.str()};
    }
    return texture;
}

void trippin::SpriteSheet::load(SDL_Renderer *renderer, const std::string &name, Scale scale) {
    std::stringstream path;
    path << "assets/" << name << "/" << name << "_" << scaleName(scale) << ".png";
    auto surface = loadImage(path.str().c_str());
    size = {surface->w, surface->h};
    texture = createTexture(renderer, surface);
    SDL_FreeSurface(surface);
}

void trippin::SpriteSheet::render(SDL_Renderer *renderer, SDL_Rect *clip, SDL_Rect *target) const {
    SDL_RenderCopyEx(renderer, texture, clip, target, 0, nullptr, SDL_FLIP_NONE);
}

const trippin::Vector<int> &trippin::SpriteSheet::getSize() const {
    return size;
}
