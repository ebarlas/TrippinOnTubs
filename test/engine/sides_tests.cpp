#include "catch2/catch.hpp"
#include "engine/Sides.h"

TEST_CASE("Sides initialization", "[sides]") {
    REQUIRE(!trippin::Sides());
    REQUIRE((int) trippin::Sides() == 0);
}

TEST_CASE("Sides set and get", "[sides]") {
    trippin::Sides c;
    c.setLeft(true);
    REQUIRE(c.testLeft());
    REQUIRE(!c.testTop());
    REQUIRE(!c.testRight());
    REQUIRE(!c.testBottom());
}