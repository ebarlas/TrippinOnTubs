#include "catch2/catch.hpp"
#include "engine/Sides.h"

TEST_CASE("Sides initialization", "[sides]") {
    REQUIRE(!trippin::Sides());
    REQUIRE((int) trippin::Sides() == 0);
}

TEST_CASE("Sides set and get", "[sides]") {
    trippin::Sides c;
    c.set(trippin::Sides::left, true);
    REQUIRE(c.test(trippin::Sides::left));
    REQUIRE(!c.test(trippin::Sides::top));
    REQUIRE(!c.test(trippin::Sides::right));
    REQUIRE(!c.test(trippin::Sides::bottom));
}