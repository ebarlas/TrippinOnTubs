#include <sstream>
#include "AutoPlay.h"
#include "sprite/Files.h"

void trippin::AutoPlay::load(const std::string &name) {
    auto json = readFile(getMapFile(name).c_str());
    auto j = nlohmann::json::parse(json);
    j.get_to(*this);
}

std::string trippin::AutoPlay::getMapFile(const std::string &name) {
    std::stringstream path;
    path << "autoplay/" << name << ".json";
    return path.str();
}

void trippin::from_json(const nlohmann::json& j, AutoPlay& ap) {
    j.at("events").get_to(ap.events);
}

void trippin::from_json(const nlohmann::json& j, GogginInputTick& uit) {
    j.at("tick").get_to(uit.tick);
    if (j.contains("jumpCharge"))
        j.at("jumpCharge").get_to(uit.jumpCharge);
    if (j.contains("jumpRelease"))
        j.at("jumpRelease").get_to(uit.jumpRelease);
    if (j.contains("duckStart"))
        j.at("duckStart").get_to(uit.duckStart);
    if (j.contains("duckEnd"))
        j.at("duckEnd").get_to(uit.duckEnd);
}