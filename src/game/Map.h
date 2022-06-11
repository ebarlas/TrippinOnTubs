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
            Point<int_fast64_t> position{};
            int_fast64_t runningAcceleration{};
            int_fast64_t risingAcceleration{};
            int_fast64_t gravity{};
            int_fast64_t fallGravity{};
            int_fast64_t mass{};
            int massFactor{};
            int_fast64_t minJumpVelocity{};
            int_fast64_t maxJumpVelocity{};
            int_fast64_t maxDuckJumpVelocity{};
            int_fast64_t minJumpChargeTime{};
            int_fast64_t maxJumpChargeTime{};
            int_fast64_t jumpGracePeriod{};
            int_fast64_t jumpSoundTimeout{};
            Point<int_fast64_t> velocity{};
            Point<int_fast64_t> terminalVelocity{};
            Point<int_fast64_t> friction{};
            int_fast64_t dustPeriod{};
            int_fast64_t duckFriction{};
            int_fast64_t coefficient{};
            bool sparkle{};
            int frame{};
            bool accelerateWhenGrounded{};
            bool stompable{};
            bool topStompable{};
            bool bottomStompable{};
            bool randFrame{};
            int_fast64_t activation{};
            bool elasticObjectCollisions{};
            int hitPoints{};
        };

        struct Layer {
            struct Object {
                Point<int_fast64_t> position{};
                std::string type;
                bool animated{};
                bool randFrame{};
                Point<int_fast64_t> velocity{};
            };

            bool anchorTop{};
            Point<int_fast64_t> size{};
            std::vector<Layer::Object> objects;
        };

        Point<int_fast64_t> universe{};
        int meterMargin{};
        std::string music;
        std::vector<Map::Object> objects;
        std::vector<Map::Layer> layers;

        void load(const std::string &name);
        static std::string getMapFile(const std::string& name);

        void rescale(int_fast64_t scale);
        void convert(int tickRate);
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
