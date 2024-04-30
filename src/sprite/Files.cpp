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

static std::optional<std::string> getPrefsFilePath(const char *fileName) {
    auto prefsPath = getPrefsPath();
    if (!prefsPath) {
        return {};
    }
    std::stringstream filePath;
    filePath << prefsPath.get() << fileName;
    return filePath.str();
}

static std::unique_ptr<SDL_RWops, int (*)(SDL_RWops *)> openPrefsFile(const char *fileName, const char *mode) {
    auto filePath = getPrefsFilePath(fileName);
    if (!filePath) {
        return std::unique_ptr<SDL_RWops, int (*)(SDL_RWops *)>{nullptr, nullptr};
    }
    return openFile(filePath->c_str(), mode);
}

std::string trippin::readFile(const char *fileName) {
    auto file = openFile(fileName, "rb");
    if (!file) {
        SDL_Log("Unable to open file %s! SDL Error: %s", fileName, SDL_GetError());
        std::terminate();
    }
    return readFile(file.get());
}

std::optional<std::string> trippin::readPrefFile(const char *fileName) {
    auto file = openPrefsFile(fileName, "rb");
    if (!file) {
        return {};
    }
    return readFile(file.get());
}

static bool write(SDL_RWops *file, const std::string &contents) {
    auto amount = SDL_RWwrite(file, contents.c_str(), 1, contents.size());
    return amount == contents.size();
}

bool trippin::writePrefFile(const char *fileName, const std::string &contents) {
    auto file = openPrefsFile(fileName, "wb");
    return file && write(file.get(), contents);
}

bool trippin::appendPrefFile(const char *fileName, const std::string &contents) {
    auto file = openPrefsFile(fileName, "a+b");
    return file && write(file.get(), contents);
}
