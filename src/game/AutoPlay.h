#ifndef TRIPPIN_AUTOPLAY_H
#define TRIPPIN_AUTOPLAY_H

#include <vector>
#include "nlohmann/json.hpp"
#include "GogginInputTick.h"

namespace trippin {
    struct AutoPlay {
        std::vector<GogginInputTick> events;
        void load(const std::string &name);
        static std::string getMapFile(const std::string& name);
    };

    void from_json(const nlohmann::json& j, AutoPlay& ap);
    void from_json(const nlohmann::json& j, GogginInputTick& uit);
}

#endif
