#include "CollisionTable.h"

void trippin::CollisionTable::rotate() {
    previous = std::move(next);
    next.clear();
}

void trippin::CollisionTable::add(const trippin::Object &a, const trippin::Object &b) {
    if (a.getId() < b.getId()) {
        next.insert({a.getId(), b.getId(), a.getVelocity(), b.getVelocity()});
    } else {
        next.insert({b.getId(), a.getId(), b.getVelocity(), a.getVelocity()});
    }
}

bool trippin::CollisionTable::test(const trippin::Object &a, const trippin::Object &b) const {
    return a.getId() < b.getId()
           ? previous.count({a.getId(), b.getId(), a.getVelocity(), b.getVelocity()})
           : previous.count({b.getId(), a.getId(), b.getVelocity(), a.getVelocity()});
}
