#include <sstream>
#include <fstream>
#include <exception>
#include "SDL.h"
#include "Files.h"

std::stringstream trippin::readFile(const char *fileName) {
    std::ifstream in(fileName);
    if (!in) {
        SDL_Log("Unable to open file %s.", fileName);
        std::terminate();
    }

    std::stringstream contents;
    contents << in.rdbuf();
    if (in.bad()) {
        SDL_Log("Error occurred reading file %s.", fileName);
        std::terminate();
    }

    return contents;
}
