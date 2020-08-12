#include "nlohmann/json.hpp"
#include "sprite/Files.h"
#include "Configuration.h"

void trippin::Configuration::load(const std::string &name) {
    auto in = readFile(getConfigFile(name).c_str());

    nlohmann::json j;
    in >> j;

    j.at("minPlayerSpan").get_to(minPlayerSpan);
    j.at("playerBaseWidth").get_to(playerBaseWidth);
}

std::string trippin::Configuration::getConfigFile(const std::string &name) {
    std::stringstream path;
    path << "config/" << name << ".json";
    return path.str();
}

int trippin::Configuration::getMinPlayerSpan() const {
    return minPlayerSpan;
}

int trippin::Configuration::getPlayerBaseWidth() const {
    return playerBaseWidth;
}
