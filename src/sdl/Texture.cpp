#include <exception>
#include "Texture.h"

trippin::Texture::Texture(SDL_Renderer *renderer, SDL_Surface *surface) {
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == nullptr) {
        SDL_Log("Unable to create texture from image! SDL Error: %s", SDL_GetError());
        std::terminate();
    }
}

trippin::Texture::~Texture() {
    SDL_DestroyTexture(texture);
}
