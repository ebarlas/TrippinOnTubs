#ifndef TRIPPIN_JSON_H
#define TRIPPIN_JSON_H

#include "nlohmann/json.hpp"
#include "engine/Point.h"

namespace trippin {
    template<class T>
    void from_json(const nlohmann::json &j, Point<T> &p) {
        j.at("x").get_to(p.x);
        j.at("y").get_to(p.y);
    }
}

#endif