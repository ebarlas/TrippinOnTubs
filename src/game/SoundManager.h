#ifndef TRIPPIN_SOUNDMANAGER_H
#define TRIPPIN_SOUNDMANAGER_H

#include <unordered_map>
#include <string>
#include "SDL_mixer.h"

namespace trippin {
    class SoundManager {
    public:
        Mix_Chunk* getEffect(const std::string &str);
        Mix_Music* getMusic(const std::string &str);
        ~SoundManager();
    private:
        std::unordered_map<std::string, Mix_Chunk*> effects;
        std::unordered_map<std::string, Mix_Music*> music;
        std::string getFileName(const std::string &str, const std::string &suffix) const;
        static Mix_Chunk* loadEffect(const std::string &str);
        static Mix_Music* loadMusic(const std::string &str);
    };
}

#endif
