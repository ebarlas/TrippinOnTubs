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
    j.at("platform").get_to(obj.platform);
    j.at("position").get_to(obj.position);
    if (j.contains("runningAcceleration"))
        j.at("runningAcceleration").get_to(obj.runningAcceleration);
    if (j.contains("risingAcceleration"))
        j.at("risingAcceleration").get_to(obj.risingAcceleration);
    if (j.contains("gravity"))
        j.at("gravity").get_to(obj.gravity);
    if (j.contains("fallGravity"))
        j.at("fallGravity").get_to(obj.fallGravity);
    if (j.contains("mass"))
        j.at("mass").get_to(obj.mass);
    if (j.contains("velocity"))
        j.at("velocity").get_to(obj.velocity);
    if (j.contains("minJumpVelocity"))
        j.at("minJumpVelocity").get_to(obj.minJumpVelocity);
    if (j.contains("maxJumpVelocity"))
        j.at("maxJumpVelocity").get_to(obj.maxJumpVelocity);
    if (j.contains("minJumpChargeTime"))
        j.at("minJumpChargeTime").get_to(obj.minJumpChargeTime);
    if (j.contains("maxJumpChargeTime"))
        j.at("maxJumpChargeTime").get_to(obj.maxJumpChargeTime);
    if (j.contains("terminalVelocity"))
        j.at("terminalVelocity").get_to(obj.terminalVelocity);
    if (j.contains("friction"))
        j.at("friction").get_to(obj.friction);
}