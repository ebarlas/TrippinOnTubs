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
    j.at("position").get_to(obj.position);
    if (j.contains("activatedAcceleration"))
        j.at("activatedAcceleration").get_to(obj.activatedAcceleration);
    if (j.contains("gravity"))
        j.at("gravity").get_to(obj.gravity);
    if (j.contains("fallGravity"))
        j.at("fallGravity").get_to(obj.fallGravity);
    if (j.contains("mass"))
        j.at("mass").get_to(obj.mass);
    if (j.contains("velocity"))
        j.at("velocity").get_to(obj.velocity);
    if (j.contains("jumpVelocity"))
        j.at("jumpVelocity").get_to(obj.jumpVelocity);
    if (j.contains("terminalVelocity"))
        j.at("terminalVelocity").get_to(obj.terminalVelocity);
    if (j.contains("friction"))
        j.at("friction").get_to(obj.friction);
}