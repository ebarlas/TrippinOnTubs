#include <iostream>
#include "nlohmann/json.hpp"
#include "SpriteMetadata.h"
#include "io/File.h"

void trippin::SpriteMetadata::load(const std::string &name) {
    std::stringstream filePath;
    filePath << "assets/sprites/" << name << "/" << name << ".json";
    auto in = readFile(filePath.str().c_str());

    nlohmann::json j;
    in >> j;

    j.at("frames").get_to(frames);
    j.at("duration").get_to(duration);
    j.at("hitBox").at("x").get_to(hitBox.x);
    j.at("hitBox").at("y").get_to(hitBox.y);
    j.at("hitBox").at("w").get_to(hitBox.w);
    j.at("hitBox").at("h").get_to(hitBox.h);
}

int trippin::SpriteMetadata::getFrames() const {
    return frames;
}

int trippin::SpriteMetadata::getDuration() const {
    return duration;
}

const trippin::Rect<int> &trippin::SpriteMetadata::getHitBox() const {
    return hitBox;
}
