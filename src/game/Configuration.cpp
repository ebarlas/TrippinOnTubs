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

void trippin::from_json(const nlohmann::json &j, Configuration &config) {
    j.at("minPlayerSpan").get_to(config.minPlayerSpan);
    j.at("playerBaseWidth").get_to(config.playerBaseWidth);
    j.at("tickPeriod").get_to(config.tickPeriod);
    j.at("map").get_to(config.map);
}