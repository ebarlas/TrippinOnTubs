#include "catch2/catch.hpp"
#include "engine/Object.h"

TEST_CASE("Object size", "[object]") {
    trippin::Object a;
    a.setPlatform(true);
    a.setSize({4, 4});
    a.setPosition({0, 0});

    REQUIRE(a.getSize().x == 4);
    REQUIRE(a.getSize().y == 4);
}