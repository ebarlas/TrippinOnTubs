#ifndef TRIPPIN_CONFIGURATION_H
#define TRIPPIN_CONFIGURATION_H

#include <string>
#include <sstream>
#include "nlohmann/json.hpp"
#include "sprite/Scale.h"

namespace trippin {
    struct Configuration {
        int minPlayerSpan;
        int playerBaseWidth;
        int tickPeriod;
        std::string map;
        std::vector<Scale> scales;
        void load(const std::string &name);
        static std::string getConfigFile(const std::string &name);

        double ticksPerSecond() const;
    };

    void from_json(const nlohmann::json &j, Configuration &config);
}

#endif
