#include <sstream>
#include "SDL_image.h"
#include "SpriteLoader.h"
#include "Files.h"

trippin::SpriteLoader::SpriteLoader(const Scale &scale) : scale(scale) {
}

SDL_Surface *trippin::SpriteLoader::loadSurface(const std::string &name, int frame) const {
    return loadSurface(getSpriteSheetFile(name, scale, frame).c_str());
}

std::vector<SDL_Surface *> trippin::SpriteLoader::loadSurfaces(const std::string &name, int frames) const {
    std::vector<SDL_Surface *> surfaces(frames);
    for (int i = 0; i < frames; i++) {
        surfaces[i] = loadSurface(name, i);
    }
    return surfaces;
}

SDL_Surface *trippin::SpriteLoader::loadSurface(const char *path) {
    auto surface = IMG_Load(path);
    if (surface == nullptr) {
        SDL_Log("Unable to load image %s. SDL_image Error: %s", path, IMG_GetError());
        std::terminate();
    }
    return surface;
}

std::string trippin::SpriteLoader::getSpriteSheetFile(const std::string &name, const Scale &scale, int frame) {
    std::stringstream path;
    path << "sprites/" << name << "/" << scale.getName() << "/" << name << "_" << frame << ".png";
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