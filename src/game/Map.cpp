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

void trippin::from_json(const nlohmann::json& j, Map& map) {
    j.at("universe").at("width").get_to(map.universe.x);
    j.at("universe").at("height").get_to(map.universe.y);
    j.at("objects").get_to(map.objects);
}

void trippin::from_json(const nlohmann::json& j, Map::Object& obj) {
    j.at("id").get_to(obj.id);
    j.at("type").get_to(obj.type);
    j.at("position").at("x").get_to(obj.position.x);
    j.at("position").at("y").get_to(obj.position.y);
}