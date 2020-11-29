#include "catch2/catch.hpp"
#include "engine/Object.h"

TEST_CASE("Object size", "[object]") {
    trippin::Object a;
    a.platform = true;
    a.size = {4, 4};
    a.position = {0.75, 0.25};
    a.syncPositions();

    REQUIRE(a.roundedBox.x == 1);
    REQUIRE(a.roundedBox.y == 0);
}