#ifndef TRIPPIN_CONFIGURATION_H
#define TRIPPIN_CONFIGURATION_H

#include <string>
#include <sstream>
#include "engine/Point.h"
#include "nlohmann/json.hpp"

namespace trippin {
    struct Configuration {
        struct Scale {
            std::string name;
            double multiplier;
            int minWidth;
        };

        struct Db {
            std::string host;
            int port;
        };

        int spiritSecondsBehind;
        int tickRate;
        int spiritClockTickPeriod;
        int activationProximity;
        int deactivationProximity;
        int shakeDuration;
        int shakeHertz;
        int shakeAmplitude;
        Point<int> healthBarSize;
        std::vector<std::string> maps;
        std::string loadMap;
        std::string trainMap;
        std::string loadAutoPlay;
        std::vector<Scale> scales;
        std::vector<std::string> prefetchSprites;
        Db db;
        void load(const std::string &name);
        static std::string getConfigFile(const std::string &name);

        double msPerTick() const;
        double ticksPerSecond() const;
        double engineTicksPerSpiritClockTick() const;
    };

    void from_json(const nlohmann::json &j, Configuration &config);
}

#endif
