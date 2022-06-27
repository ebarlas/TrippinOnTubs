#include "SDL_image.h"
#include "SpriteLoader.h"
#include "Files.h"

trippin::SpriteLoader::SpriteLoader(const Scale &scale) : scale(scale) {
}

SDL_Surface *trippin::SpriteLoader::loadSurface(const std::string &name) const {
    return loadSurface(getSpriteSheetFile(name, scale).c_str());
}

SDL_Surface *trippin::SpriteLoader::loadSurface(const char *path) {
    auto surface = IMG_Load(path);
    if (surface == nullptr) {
        SDL_Log("Unable to load image %s. SDL_image Error: %s", path, IMG_GetError());
        std::terminate();
    }
    return surface;
}

std::string trippin::SpriteLoader::getSpriteSheetFile(const std::string &name, const Scale &scale) {
    std::stringstream path;
    path << "sprites/" << name << "/" << name << "_" << scale.getName() << ".png";
    return path.str();
}

SDL_Texture *trippin::SpriteLoader::createTexture(SDL_Renderer *renderer, SDL_Surface *surface) {
    auto texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == nullptr) {
        SDL_Log("Unable to create texture from image. SDL Error: %s", SDL_GetError());
        std::terminate();
    }
    return texture;
}

const trippin::Scale &trippin::SpriteLoader::getScale() const {
    return scale;
}