#include <sstream>
#include "SDL.h"
#include "SDL_image.h"
#include "SpriteException.h"

trippin::SpriteException::SpriteException(std::string &&message) : message(std::move(message)) {}

const std::string &trippin::SpriteException::getMessage() const {
    return message;
}

void trippin::SpriteException::throwSdlError(std::string &&prefix) {
    std::stringstream message;
    message << prefix << SDL_GetError();
    throw SpriteException{message.str()};
}

void trippin::SpriteException::throwSdlImageError(std::string &&prefix) {
    std::stringstream message;
    message << prefix << IMG_GetError();
    throw SpriteException{message.str()};
}
