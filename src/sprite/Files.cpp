#include <sstream>
#include <exception>
#include <memory>
#include "SDL.h"
#include "Files.h"

const char *name = "trippinontubs";

constexpr int fileBufferSize = 1'024;

static std::string readFile(SDL_RWops *file) {
    std::stringstream contents;
    char buffer[fileBufferSize];
    size_t n;
    while ((n = SDL_RWread(file, buffer, sizeof(char), fileBufferSize)) > 0) {
        contents.write(buffer, static_cast<long>(n));
    }
    return contents.str();
}

static std::unique_ptr<SDL_RWops, int (*)(SDL_RWops *)> openFile(const char *file, const char *mode) {
    return std::unique_ptr<SDL_RWops, int (*)(SDL_RWops *)>{SDL_RWFromFile(file, mode), SDL_RWclose};
}

static std::unique_ptr<char, void (*)(void *)> getPrefsPath() {
    return std::unique_ptr<char, void (*)(void *)>{SDL_GetPrefPath(name, name), SDL_free};
}

static std::string getPrefsFilePath(const char *fileName) {
    auto prefsPath = getPrefsPath();
    std::stringstream filePath;
    filePath << prefsPath.get() << fileName;
    return filePath.str();
}

std::string trippin::readFile(const char *fileName) {
    auto file = openFile(fileName, "rb");
    if (file == nullptr) {
        SDL_Log("Unable to open file %s! SDL Error: %s", fileName, SDL_GetError());
        std::terminate();
    }
    return readFile(file.get());
}

std::optional<std::string> trippin::readPrefFile(const char *fileName) {
    auto filePath = getPrefsFilePath(fileName);
    auto file = openFile(filePath.c_str(), "rb");
    if (file == nullptr) {
        return {};
    }
    return readFile(file.get());
}

bool trippin::writePrefFile(const char *fileName, const std::string &contents) {
    auto filePath = getPrefsFilePath(fileName);
    auto file = openFile(filePath.c_str(), "wb");
    if (file == nullptr) {
        return false;
    }
    auto amount = SDL_RWwrite(file.get(), contents.c_str(), 1, contents.size());
    return amount == contents.size();
}
