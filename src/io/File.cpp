#include "SDL.h"
#include "File.h"
#include "exception/TubsException.h"

static constexpr int fileBufferSize = 128;

std::stringstream trippin::readFile(const char *fileName) {
    auto file = SDL_RWFromFile(fileName, "rb");
    if (file == nullptr) {
        std::stringstream message;
        message << "Unable to open file " << fileName << ". SDL error: " << SDL_GetError();
        throw TubsException{message.str()};
    }

    std::stringstream contents;
    char buffer[fileBufferSize];
    int n;
    while ((n = SDL_RWread(file, buffer, sizeof(char), fileBufferSize)) > 0) {
        contents.write(buffer, n);
    }

    SDL_RWclose(file);

    return contents;
}