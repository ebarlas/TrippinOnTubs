#ifndef TRIPPIN_MAP_H
#define TRIPPIN_MAP_H

#include <vector>
#include "nlohmann/json.hpp"
#include "engine/Point.h"

namespace trippin {
    struct Map {
        struct Object {
            int id{};
            std::string type;
            Point<double> position;
            double activatedAcceleration{};
            double gravity{};
            double fallGravity{};
            double mass{};
            double jumpVelocity;
            Point<double> velocity{};
            Point<double> terminalVelocity;
            Point<double> friction;
        };

        Point<int> universe;
        std::vector<Map::Object> objects;

        void load(const std::string &name);
        static std::string getMapFile(const std::string& name);
    };

    void from_json(const nlohmann::json& j, Map& map);
    void from_json(const nlohmann::json& j, Map::Object& obj);
}

#endif
