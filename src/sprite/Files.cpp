#include <sstream>
#include <exception>
#include "SDL.h"
#include "Files.h"

std::stringstream trippin::readFile(const char *fileName) {
    auto file = SDL_RWFromFile(fileName, "rb");
    if (file == nullptr) {
        SDL_Log("Unable to open file %s! SDL Error: %s", fileName, SDL_GetError());
        std::terminate();
    }

    std::stringstream contents;
    char buffer[fileBufferSize];
    size_t n;
    while ((n = SDL_RWread(file, buffer, sizeof(char), fileBufferSize)) > 0) {
        contents.write(buffer, static_cast<long>(n));
    }

    SDL_RWclose(file);

    return contents;
}
