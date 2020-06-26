#include "sides.h" 

trippin::Sides &trippin::Sides::operator|=(const Sides &s) {
    sides |= s.sides;
    return *this;
}

trippin::Sides::operator bool() const {
    return sides.any();
}

trippin::Sides::operator int() const {
    return sides.count();
}

int trippin::Sides::count() const {
    return sides.count();
}

void trippin::Sides::set(trippin::Sides::Side side, bool on) {
    sides.set(side, on);
}

bool trippin::Sides::test(trippin::Sides::Side side) const {
    return sides.test(side);
}

bool trippin::Sides::testLeft() const {
    return test(Side::left);
}

bool trippin::Sides::testTop() const {
    return test(Side::top);
}

bool trippin::Sides::testRight() const {
    return test(Side::right);
}

bool trippin::Sides::testBottom() const {
    return test(Side::bottom);
}

void trippin::Sides::setLeft(bool on) {
    set(Side::left, on);
}

void trippin::Sides::setTop(bool on) {
    set(Side::top, on);
}

void trippin::Sides::setRight(bool on) {
    set(Side::right, on);
}

void trippin::Sides::setBottom(bool on) {
    set(Side::bottom, on);
}