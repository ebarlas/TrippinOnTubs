#include <sstream>
#include "SDL.h"
#include "SoundManager.h"

trippin::SoundManager::~SoundManager() {
    for (auto &ptr : effects) {
        Mix_FreeChunk(ptr.second);
    }
    for (auto &ptr : music) {
        Mix_FreeMusic(ptr.second);
    }
}

Mix_Chunk *trippin::SoundManager::getEffect(const std::string &str) {
    auto name = getFileName(str, ".wav");
    auto it = effects.find(name);
    return it == effects.end()
           ? effects[name] = loadEffect(name)
           : it->second;
}

Mix_Music *trippin::SoundManager::getMusic(const std::string &str) {
    auto name = getFileName(str, ".mp3");
    auto it = music.find(name);
    return it == music.end()
           ? music[name] = loadMusic(name)
           : it->second;
}

std::string trippin::SoundManager::getFileName(const std::string &str, const std::string &suffix) const {
    std::stringstream path;
    path << "sounds/" << str << suffix;
    return path.str();
}

Mix_Chunk *trippin::SoundManager::loadEffect(const std::string &str) {
    auto effect = Mix_LoadWAV(str.c_str());
    if (effect == nullptr) {
        SDL_Log("Mixer chunk could not be loaded. SDL error: %s", Mix_GetError());
        std::terminate();
    }
    return effect;
}

Mix_Music *trippin::SoundManager::loadMusic(const std::string &str) {
    auto music = Mix_LoadMUS(str.c_str());
    if (music == nullptr) {
        SDL_Log("Music could not be loaded. SDL error: %s", Mix_GetError());
        std::terminate();
    }
    return music;
}
