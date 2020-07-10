#include "catch2/catch.hpp"
#include "engine/Point.h"

TEST_CASE("Point initialization", "[vector]")
{
    REQUIRE(trippin::Point<int>{2, 4}.x == 2);
    REQUIRE(trippin::Point<int>{2, 4}.y == 4);
}