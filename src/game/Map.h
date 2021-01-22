#ifndef TRIPPIN_MAP_H
#define TRIPPIN_MAP_H

#include <vector>
#include "nlohmann/json.hpp"
#include "engine/Point.h"
#include "engine/Rect.h"

namespace trippin {
    struct Map {
        struct Object {
            int id{};
            std::string type;
            bool platform;
            Point<double> position;
            double runningAcceleration{};
            double risingAcceleration{};
            double gravity{};
            double fallGravity{};
            double mass{};
            double minJumpVelocity;
            double maxJumpVelocity;
            int minJumpChargeTime;
            int maxJumpChargeTime;
            int jumpGracePeriod;
            Point<double> velocity{};
            Point<double> terminalVelocity;
            Point<double> friction;
        };

        struct Layer {
            struct Object {
                Point<int> position;
                std::string type;
            };

            bool anchorTop{};
            Point<int> size;
            std::vector<Layer::Object> objects;
        };

        Point<int> universe;
        std::vector<Map::Object> objects;
        std::vector<Map::Layer> layers;

        void load(const std::string &name);
        static std::string getMapFile(const std::string& name);
    };

    template<class T>
    void from_json(const nlohmann::json& j, Point<T>& p) {
        j.at("x").get_to(p.x);
        j.at("y").get_to(p.y);
    }

    void from_json(const nlohmann::json& j, Map& map);
    void from_json(const nlohmann::json& j, Map::Object& obj);
    void from_json(const nlohmann::json& j, Map::Layer& layer);
    void from_json(const nlohmann::json& j, Map::Layer::Object& obj);
}

#endif
