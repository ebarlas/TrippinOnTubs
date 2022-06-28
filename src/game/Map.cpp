#include "nlohmann/json.hpp"
#include "sprite/Files.h"
#include "Map.h"

void trippin::Map::load(const std::string &name) {
    auto in = readFile(getMapFile(name).c_str());
    nlohmann::json j;
    in >> j;
    j.get_to(*this);
}

std::string trippin::Map::getMapFile(const std::string &name) {
    std::stringstream path;
    path << "maps/" << name << ".json";
    return path.str();
}

void trippin::Map::rescale(int scale) {
    universe *= scale;

    for (auto &obj: objects) {
        obj.position *= scale;
        obj.activation *= scale;
    }

    for (auto &layer: layers) {
        layer.size *= scale;
        for (auto &obj: layer.objects) {
            obj.position *= scale;
        }
    }
}

void trippin::from_json(const nlohmann::json &j, Map &map) {
    j.at("universe").at("width").get_to(map.universe.x);
    j.at("universe").at("height").get_to(map.universe.y);
    j.at("music").get_to(map.music);
    j.at("objects").get_to(map.objects);
    j.at("layers").get_to(map.layers);
}

void trippin::from_json(const nlohmann::json &j, Map::Object &obj) {
    j.at("id").get_to(obj.id);
    j.at("type").get_to(obj.type);
    j.at("position").get_to(obj.position);
    if (j.contains("lane"))
        j.at("lane").get_to(obj.lane);
    if (j.contains("sparkle"))
        j.at("sparkle").get_to(obj.sparkle);
    if (j.contains("activation"))
        j.at("activation").get_to(obj.activation);
}

void trippin::from_json(const nlohmann::json &j, Map::Layer &layer) {
    j.at("width").get_to(layer.size.x);
    j.at("height").get_to(layer.size.y);
    j.at("objects").get_to(layer.objects);
    if (j.contains("anchorTop"))
        j.at("anchorTop").get_to(layer.anchorTop);
}

void trippin::from_json(const nlohmann::json &j, Map::LayerObject &obj) {
    j.at("type").get_to(obj.type);
    j.at("position").get_to(obj.position);
}