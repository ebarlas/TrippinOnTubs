#ifndef TRIPPIN_MAP_H
#define TRIPPIN_MAP_H

#include <vector>
#include "game/Json.h"

namespace trippin {
    struct Map {
        struct Object {
            int id{};
            int lane{};
            std::string type;
            Point<int> position;
            bool sparkle{};
            int activation{};
        };

        struct LayerObject {
            Point<int> position;
            std::string type;
        };

        struct Layer {
            bool anchorTop{};
            Point<int> size;
            std::vector<LayerObject> objects;
        };

        Point<int> universe;
        std::string music;
        std::vector<Map::Object> objects;
        std::vector<Map::Layer> layers;

        void load(const std::string &name);
        static std::string getMapFile(const std::string &name);

        void rescale(int scale);
        void convert(double tickRate);
    };

    void from_json(const nlohmann::json &j, Map &map);
    void from_json(const nlohmann::json &j, Map::Object &obj);
    void from_json(const nlohmann::json &j, Map::Layer &layer);
    void from_json(const nlohmann::json &j, Map::LayerObject &obj);
}

#endif
