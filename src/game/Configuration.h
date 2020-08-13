#ifndef TRIPPIN_CONFIGURATION_H
#define TRIPPIN_CONFIGURATION_H

#include <string>
#include <sstream>
#include "nlohmann/json.hpp"

namespace trippin {
    struct Configuration {
        int minPlayerSpan;
        int playerBaseWidth;
        std::string map;
        void load(const std::string &name);
        static std::string getConfigFile(const std::string &name);
    };

    void from_json(const nlohmann::json &j, Configuration &config);
}

#endif
