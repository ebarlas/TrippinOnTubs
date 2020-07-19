#include "Partition.h"

void trippin::Partition::add(Object *obj) {
    auto &vec = obj->platform ? platforms : objects;
    if (std::find(vec.begin(), vec.end(), obj) == vec.end()) {
        vec.push_back(obj);
    }
}

void trippin::Partition::remove(const trippin::Object *obj) {
    auto &vec = obj->platform ? platforms : objects;
    auto it = std::find(vec.begin(), vec.end(), obj);
    vec.erase(it);
}