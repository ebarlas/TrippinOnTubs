#ifndef TRIPPINONTUBS_FILES_H
#define TRIPPINONTUBS_FILES_H

#include <sstream>
#include "SDL.h"
#include "Scale.h"
#include "SpriteType.h"
#include "SpriteException.h"

namespace trippin {
    inline std::string getMetadataFile(const std::string& name) {
        std::stringstream path;
        path << "sprites/" << name << "/" << name << ".json";
        return path.str();
    }

    inline std::string getSpriteSheetFile(const std::string& name, Scale scale) {
        std::stringstream path;
        path << "sprites/" << name << "/" << name << "_" << scaleName(scale) << ".png";
        return path.str();
    }

    constexpr int fileBufferSize = 128;

    inline std::stringstream readFile(const char *fileName) {
        auto file = SDL_RWFromFile(fileName, "rb");
        if (file == nullptr) {
            SpriteException::throwSdlError(std::string{"Unable to open file "} + fileName + ". SDL error: ");
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
}

#endif
