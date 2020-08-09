#include "SDL_image.h"
#include "Scale.h"
#include "SpriteSheet.h"
#include "Files.h"
#include "sdl/Surface.h"

trippin::SpriteSheet::SpriteSheet(SDL_Renderer *ren, const std::string &name, Scale scale) : renderer(ren) {
    Surface surface{getSpriteSheetFile(name, scale).c_str()};
    size = {surface.surface->w, surface.surface->h};
    texture = std::make_unique<Texture>(renderer, surface.surface);
}

std::string trippin::SpriteSheet::getSpriteSheetFile(const std::string& name, Scale scale) {
    std::stringstream path;
    path << "sprites/" << name << "/" << name << "_" << scaleName(scale) << ".png";
    return path.str();
}

void trippin::SpriteSheet::render(SDL_Rect *clip, SDL_Rect *target) const {
    SDL_RenderCopyEx(renderer, texture->texture, clip, target, 0, nullptr, SDL_FLIP_NONE);
}

trippin::Point<int> trippin::SpriteSheet::getSize() const {
    return size;
}
