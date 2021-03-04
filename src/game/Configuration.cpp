#include "nlohmann/json.hpp"
#include "sprite/Files.h"
#include "engine/Convert.h"
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
    return 1'000.0 / tickPeriod;
}

double trippin::Configuration::engineTicksPerSpiritClockTick() const {
    return toDouble(spiritClockTickPeriod) / tickPeriod;
}

double trippin::Configuration::pointsPerTick() const {
    return pointsPerSecond / ticksPerSecond();
}

void trippin::from_json(const nlohmann::json &j, Configuration &config) {
    j.at("spiritSecondsBehind").get_to(config.spiritSecondsBehind);
    j.at("tickPeriod").get_to(config.tickPeriod);
    j.at("spiritClockTickPeriod").get_to(config.spiritClockTickPeriod);
    j.at("activationProximity").get_to(config.activationProximity);
    j.at("deactivationProximity").get_to(config.deactivationProximity);
    j.at("pointsPerSecond").get_to(config.pointsPerSecond);
    j.at("map").get_to(config.map);
    j.at("loadMap").get_to(config.loadMap);
    j.at("loadAutoPlay").get_to(config.loadAutoPlay);

    for (auto &elem : j.at("scales")) {
        std::string name;
        double multiplier;
        int minWidth;
        elem.at("name").get_to(name);
        elem.at("multiplier").get_to(multiplier);
        elem.at("minWidth").get_to(minWidth);
        config.scales.push_back({std::move(name), multiplier, minWidth});
    }
}