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

void trippin::Map::rescale(int_fast64_t scale) {
    universe *= scale;

    for (auto &obj: objects) {
        obj.position *= scale;
        obj.runningAcceleration *= scale;
        obj.risingAcceleration *= scale;
        obj.gravity *= scale;
        obj.fallGravity *= scale;
        obj.velocity *= scale;
        obj.minJumpVelocity *= scale;
        obj.maxJumpVelocity *= scale;
        obj.maxDuckJumpVelocity *= scale;
        obj.terminalVelocity *= scale;
        obj.friction *= scale;
        obj.duckFriction *= scale;
        obj.activation *= scale;
    }

    for (auto &layer: layers) {
        layer.size *= scale;
        for (auto &obj: layer.objects) {
            obj.position *= scale;
            obj.velocity *= scale;
        }
    }
}

void trippin::Map::convert(int ticksPerSecond) {
    auto ticksPerSecondSq = ticksPerSecond * ticksPerSecond;
    auto toTicks = [ticksPerSecond](int n) {
        return n * ticksPerSecond / 1'000;
    };

    for (auto &obj: objects) {
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
        obj.minJumpChargeTime = toTicks(obj.minJumpChargeTime);
        obj.maxJumpChargeTime = toTicks(obj.maxJumpChargeTime);
        obj.jumpGracePeriod = toTicks(obj.jumpGracePeriod);
        obj.jumpSoundTimeout = toTicks(obj.jumpSoundTimeout);
        obj.dustPeriod = toTicks(obj.dustPeriod);
        obj.duckFriction /= ticksPerSecondSq;
    }
}

void trippin::from_json(const nlohmann::json &j, Map &map) {
    j.at("universe").at("width").get_to(map.universe.x);
    j.at("universe").at("height").get_to(map.universe.y);
    j.at("meterMargin").get_to(map.meterMargin);
    j.at("music").get_to(map.music);
    j.at("objects").get_to(map.objects);
    j.at("layers").get_to(map.layers);
}

void trippin::from_json(const nlohmann::json &j, Map::Object &obj) {
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
    if (j.contains("massFactor"))
        j.at("massFactor").get_to(obj.massFactor);
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
    if (j.contains("jumpSoundTimeout"))
        j.at("jumpSoundTimeout").get_to(obj.jumpSoundTimeout);
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
    if (j.contains("frame"))
        j.at("frame").get_to(obj.frame);
    if (j.contains("accelerateWhenGrounded"))
        j.at("accelerateWhenGrounded").get_to(obj.accelerateWhenGrounded);
    if (j.contains("stompable"))
        j.at("stompable").get_to(obj.stompable);
    if (j.contains("topStompable"))
        j.at("topStompable").get_to(obj.topStompable);
    if (j.contains("bottomStompable"))
        j.at("bottomStompable").get_to(obj.bottomStompable);
    if (j.contains("randFrame"))
        j.at("randFrame").get_to(obj.randFrame);
    if (j.contains("elasticObjectCollisions"))
        j.at("elasticObjectCollisions").get_to(obj.elasticObjectCollisions);
    if (j.contains("hitPoints"))
        j.at("hitPoints").get_to(obj.hitPoints);
}

void trippin::from_json(const nlohmann::json &j, Map::Layer &layer) {
    j.at("width").get_to(layer.size.x);
    j.at("height").get_to(layer.size.y);
    j.at("objects").get_to(layer.objects);
    if (j.contains("anchorTop"))
        j.at("anchorTop").get_to(layer.anchorTop);
}

void trippin::from_json(const nlohmann::json &j, Map::Layer::Object &obj) {
    j.at("type").get_to(obj.type);
    j.at("position").get_to(obj.position);
    if (j.contains("animated"))
        j.at("animated").get_to(obj.animated);
    if (j.contains("randFrame"))
        j.at("randFrame").get_to(obj.randFrame);
    if (j.contains("velocity"))
        j.at("velocity").get_to(obj.velocity);
}