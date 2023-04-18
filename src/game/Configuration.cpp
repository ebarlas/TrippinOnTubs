#include <sstream>
#include "nlohmann/json.hpp"
#include "sprite/Files.h"
#include "Configuration.h"

void trippin::Configuration::load(const std::string &name) {
    auto in = readFile(getConfigFile(name).c_str());
    nlohmann::json j;
    in >> j;
    j.get_to(*this);
}

std::string trippin::Configuration::getConfigFile(const std::string &name) {
    std::stringstream path;
    path << "config/" << name << ".json";
    return path.str();
}

double trippin::Configuration::ticksPerSecond() const {
    return tickRate;
}

double trippin::Configuration::engineTicksPerSpiritClockTick() const {
    return spiritClockTickPeriod / msPerTick();
}

double trippin::Configuration::msPerTick() const {
    return 1000.0 / tickRate;
}

void trippin::Configuration::rescale(const trippin::Scale &sc) {
    auto df = sc.getDeviceFactor();
    healthBarSize *= df;
    meterMargin *= df;
    scrollPixelsPerSecond *= df;

    auto ef = sc.getEngineFactor();
    shakeAmplitude *= ef;
    activationProximity *= ef;
    deactivationProximity *= ef;

    for (auto &obj: objects) {
        obj.runningAcceleration *= ef;
        obj.risingAcceleration *= ef;
        obj.gravity *= ef;
        obj.fallGravity *= ef;
        obj.velocity *= ef;
        obj.minJumpVelocity *= ef;
        obj.maxJumpVelocity *= ef;
        obj.maxDuckJumpVelocity *= ef;
        obj.terminalVelocity *= ef;
        obj.friction *= ef;
        obj.duckFriction *= ef;
    }

    for (auto &obj: layerObjects) {
        obj.velocity *= ef;
    }

    auto ticksPerSecond = tickRate;
    auto tickPeriod = 1000.0 / ticksPerSecond;
    auto ticksPerSecondSq = ticksPerSecond * ticksPerSecond;

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
        obj.minJumpChargeTime = static_cast<int>(std::round(obj.minJumpChargeTime / tickPeriod));
        obj.maxJumpChargeTime = static_cast<int>(std::round(obj.maxJumpChargeTime / tickPeriod));
        obj.jumpSoundTimeout = static_cast<int>(std::round(obj.jumpSoundTimeout / tickPeriod));
        obj.dustPeriod = static_cast<int>(std::round(obj.dustPeriod / tickPeriod));
        obj.duckFriction /= ticksPerSecondSq;
    }

    for (auto &obj: layerObjects) {
        obj.velocity /= ticksPerSecond;
    }
}

const trippin::Configuration::Object *trippin::Configuration::findObject(const std::string &type) const {
    auto fn = [&type](const Configuration::Object &configObj) { return type == configObj.type; };
    auto it = std::find_if(objects.begin(), objects.end(), fn);
    return it == objects.end() ? nullptr : &(*it);
}

const trippin::Configuration::LayerObject *trippin::Configuration::findLayerObject(const std::string &type) const {
    auto fn = [&type](const Configuration::LayerObject &configObj) { return type == configObj.type; };
    auto it = std::find_if(layerObjects.begin(), layerObjects.end(), fn);
    return it == layerObjects.end() ? nullptr : &(*it);
}

void trippin::from_json(const nlohmann::json &j, Configuration &config) {
    j.at("version").at("major").get_to(config.version.major);
    j.at("version").at("minor").get_to(config.version.minor);
    if (j.contains("windowSize")) {
        j.at("windowSize").at("x").get_to(config.windowSize.x);
        j.at("windowSize").at("y").get_to(config.windowSize.y);
    }
    j.at("extraLives").get_to(config.extraLives);
    j.at("spiritSecondsBehind").get_to(config.spiritSecondsBehind);
    j.at("tickRate").get_to(config.tickRate);
    j.at("spiritClockTickPeriod").get_to(config.spiritClockTickPeriod);
    j.at("activationProximity").get_to(config.activationProximity);
    j.at("deactivationProximity").get_to(config.deactivationProximity);
    j.at("shakeDuration").get_to(config.shakeDuration);
    j.at("shakeHertz").get_to(config.shakeHertz);
    j.at("shakeAmplitude").get_to(config.shakeAmplitude);
    j.at("meterMargin").get_to(config.meterMargin);
    j.at("fastReplayFactor").get_to(config.fastReplayFactor);
    j.at("scrollPixelsPerSecond").get_to(config.scrollPixelsPerSecond);
    j.at("minComboHits").get_to(config.minComboHits);
    j.at("pointsPerObject").get_to(config.pointsPerObject);
    j.at("pointsPerHitPoint").get_to(config.pointsPerHitPoint);
    j.at("pointsPerGroup").get_to(config.pointsPerGroup);
    j.at("pointsPerMember").get_to(config.pointsPerMember);
    j.at("pointsPerComboHit").get_to(config.pointsPerComboHit);
    j.at("timeBonusPoints").get_to(config.timeBonusPoints);
    j.at("timeBonusSeconds").get_to(config.timeBonusSeconds);
    j.at("tubBonusPoints").get_to(config.tubBonusPoints);
    j.at("sparkleTubBonusPoints").get_to(config.sparkleTubBonusPoints);
    j.at("collisionDurationMilliseconds").get_to(config.collisionDurationMilliseconds);
    j.at("collisionFlashMilliseconds").get_to(config.collisionFlashMilliseconds);
    j.at("collisionCooldownMilliseconds").get_to(config.collisionCooldownMilliseconds);
    j.at("stompSound").get_to(config.stompSound);
    j.at("jumpSound").get_to(config.jumpSound);
    j.at("knockoutVelocityFactor").get_to(config.knockoutVelocityFactor);
    j.at("healthBarVerticalClearanceFactor").get_to(config.healthBarVerticalClearanceFactor);
    j.at("dustJumpFactor").get_to(config.dustJumpFactor);
    j.at("dustLandFactor").get_to(config.dustLandFactor);
    j.at("dustRunFactor").get_to(config.dustRunFactor);
    j.at("maxConsecutiveJumps").get_to(config.maxConsecutiveJumps);
    j.at("healthBarSize").at("x").get_to(config.healthBarSize.x);
    j.at("healthBarSize").at("y").get_to(config.healthBarSize.y);
    j.at("maps").get_to(config.maps);
    j.at("loadMap").get_to(config.loadMap);
    j.at("trainMap").get_to(config.trainMap);
    j.at("loadAutoPlay").get_to(config.loadAutoPlay);
    j.at("prefetchSprites").get_to(config.prefetchSprites);
    j.at("db").at("host").get_to(config.db.host);
    j.at("db").at("port").get_to(config.db.port);
    if (j.contains("objects")) {
        j.at("objects").get_to(config.objects);
    }
    if (j.contains("layerObjects")) {
        j.at("layerObjects").get_to(config.layerObjects);
    }
    for (auto &elem: j.at("scales")) {
        std::string name;
        int multiplier;
        int minWidth;
        elem.at("name").get_to(name);
        elem.at("multiplier").get_to(multiplier);
        elem.at("minWidth").get_to(minWidth);
        config.scales.push_back({std::move(name), multiplier, minWidth});
    }
}

void trippin::from_json(const nlohmann::json &j, Configuration::Object &obj) {
    j.at("type").get_to(obj.type);
    j.at("platform").get_to(obj.platform);
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
    if (j.contains("jumpSoundTimeout"))
        j.at("jumpSoundTimeout").get_to(obj.jumpSoundTimeout);
    if (j.contains("terminalVelocity"))
        j.at("terminalVelocity").get_to(obj.terminalVelocity);
    if (j.contains("friction"))
        j.at("friction").get_to(obj.friction);
    if (j.contains("dustPeriod"))
        j.at("dustPeriod").get_to(obj.dustPeriod);
    if (j.contains("duckFriction"))
        j.at("duckFriction").get_to(obj.duckFriction);
    if (j.contains("coefficient"))
        j.at("coefficient").get_to(obj.coefficient);
    if (j.contains("sparkle"))
        j.at("sparkle").get_to(obj.sparkle);
    if (j.contains("accelerateWhenGrounded"))
        j.at("accelerateWhenGrounded").get_to(obj.accelerateWhenGrounded);
    if (j.contains("randFrame"))
        j.at("randFrame").get_to(obj.randFrame);
    if (j.contains("elasticObjectCollisions"))
        j.at("elasticObjectCollisions").get_to(obj.elasticObjectCollisions);
    if (j.contains("hitPoints"))
        j.at("hitPoints").get_to(obj.hitPoints);
}

void trippin::from_json(const nlohmann::json &j, Configuration::LayerObject &obj) {
    j.at("type").get_to(obj.type);
    if (j.contains("randFrame"))
        j.at("randFrame").get_to(obj.randFrame);
    if (j.contains("velocity"))
        j.at("velocity").get_to(obj.velocity);
}