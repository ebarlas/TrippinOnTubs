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

void trippin::Map::rescale(double target) {
    // if target=0.25 and scale=0.25, divisor=1.0
    // if target=0.25 and scale=1.0, divisor=4.0
    double divisor = scale / target;

    universe.x /= divisor;
    universe.y /= divisor;
    meterMargin = static_cast<int>(std::round(meterMargin / divisor));

    for (auto &obj : objects) {
        obj.position.x /= divisor;
        obj.position.y /= divisor;
        obj.runningAcceleration /= divisor;
        obj.risingAcceleration /= divisor;
        obj.gravity /= divisor;
        obj.fallGravity /= divisor;
        obj.velocity.x /= divisor;
        obj.velocity.y /= divisor;
        obj.minJumpVelocity /= divisor;
        obj.maxJumpVelocity /= divisor;
        obj.maxDuckJumpVelocity /= divisor;
        obj.terminalVelocity.x /= divisor;
        obj.terminalVelocity.y /= divisor;
        obj.friction.x /= divisor;
        obj.friction.y /= divisor;
    }

    for (auto &layer : layers) {
        layer.size.x /= divisor;
        layer.size.y /= divisor;
        for (auto &obj : layer.objects) {
            obj.position.x /= divisor;
            obj.position.y /= divisor;
        }
    }
}

void trippin::Map::convert(int tickPeriod) {
    auto ticksPerSecond = 1'000.0 / tickPeriod;
    auto ticksPerSecondSq = ticksPerSecond * ticksPerSecond;

    for (auto &obj : objects) {
        obj.gravity /= ticksPerSecondSq;
        obj.fallGravity /= ticksPerSecondSq;
        obj.friction /= ticksPerSecondSq;
        obj.velocity /= ticksPerSecond;
        obj.terminalVelocity /= ticksPerSecond;
        obj.runningAcceleration /= ticksPerSecondSq;
        obj.risingAcceleration /= ticksPerSecondSq;
        obj.minJumpVelocity /= ticksPerSecond;
        obj.maxJumpVelocity /= ticksPerSecond;
        obj.maxDuckJumpVelocity /= ticksPerSecond;
        obj.minJumpChargeTime /= tickPeriod;
        obj.maxJumpChargeTime /= tickPeriod;
        obj.jumpGracePeriod /= tickPeriod;
        obj.dustPeriod /= tickPeriod;
        obj.duckFriction /= ticksPerSecondSq;
    }
}

void trippin::from_json(const nlohmann::json& j, Map& map) {
    j.at("universe").at("width").get_to(map.universe.x);
    j.at("universe").at("height").get_to(map.universe.y);
    j.at("scale").get_to(map.scale);
    j.at("meterMargin").get_to(map.meterMargin);
    j.at("objects").get_to(map.objects);
    j.at("layers").get_to(map.layers);
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
    if (j.contains("maxDuckJumpVelocity"))
        j.at("maxDuckJumpVelocity").get_to(obj.maxDuckJumpVelocity);
    if (j.contains("minJumpChargeTime"))
        j.at("minJumpChargeTime").get_to(obj.minJumpChargeTime);
    if (j.contains("maxJumpChargeTime"))
        j.at("maxJumpChargeTime").get_to(obj.maxJumpChargeTime);
    if (j.contains("jumpGracePeriod"))
        j.at("jumpGracePeriod").get_to(obj.jumpGracePeriod);
    if (j.contains("terminalVelocity"))
        j.at("terminalVelocity").get_to(obj.terminalVelocity);
    if (j.contains("friction"))
        j.at("friction").get_to(obj.friction);
    if (j.contains("lane"))
        j.at("lane").get_to(obj.lane);
    if (j.contains("dustPeriod"))
        j.at("dustPeriod").get_to(obj.dustPeriod);
    if (j.contains("duckFriction"))
        j.at("duckFriction").get_to(obj.duckFriction);
    if (j.contains("coefficient"))
        j.at("coefficient").get_to(obj.coefficient);
    if (j.contains("sparkle"))
        j.at("sparkle").get_to(obj.sparkle);
}

void trippin::from_json(const nlohmann::json& j, Map::Layer& layer) {
    j.at("width").get_to(layer.size.x);
    j.at("height").get_to(layer.size.y);
    j.at("objects").get_to(layer.objects);
    if (j.contains("anchorTop"))
        j.at("anchorTop").get_to(layer.anchorTop);
}

void trippin::from_json(const nlohmann::json& j, Map::Layer::Object& obj) {
    j.at("type").get_to(obj.type);
    j.at("position").get_to(obj.position);
}