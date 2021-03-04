#ifndef TRIPPIN_MAP_H
#define TRIPPIN_MAP_H

#include <vector>
#include "nlohmann/json.hpp"
#include "engine/Point.h"
#include "engine/Rect.h"
#include "sprite/Scale.h"

namespace trippin {
    struct Map {
        struct Object {
            int id{};
            int lane{};
            std::string type;
            bool platform;
            Point<double> position;
            double runningAcceleration{};
            double risingAcceleration{};
            double gravity{};
            double fallGravity{};
            double mass{};
            double massFactor{};
            double minJumpVelocity;
            double maxJumpVelocity;
            double maxDuckJumpVelocity;
            int minJumpChargeTime;
            int maxJumpChargeTime;
            int jumpGracePeriod;
            int jumpSoundTimeout;
            Point<double> velocity{};
            Point<double> terminalVelocity{};
            Point<double> friction{};
            int dustPeriod{};
            double duckFriction{};
            double coefficient{};
            bool sparkle{};
            int frame{};
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
        double scale;
        int meterMargin;
        std::string music;
        std::vector<Map::Object> objects;
        std::vector<Map::Layer> layers;

        void load(const std::string &name);
        static std::string getMapFile(const std::string& name);

        void rescale(double scale);
        void convert(int tickPeriod);
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
