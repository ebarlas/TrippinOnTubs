#include "catch2/catch.hpp"
#include "engine/vector.h"

TEST_CASE("Vector initialization", "[vector]")
{
    REQUIRE(trippin::Vector<int>{2, 4}.x == 2);
    REQUIRE(trippin::Vector<int>{2, 4}.y == 4);
}