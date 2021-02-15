#include "catch2/catch.hpp"
#include "game/Map.h"

TEST_CASE("Load test map", "[map]")
{
    trippin::Map map{};
    map.load("map_test");
    REQUIRE(map.objects.size() > 0);
    REQUIRE(map.universe.x > 0);
    REQUIRE(map.universe.y > 0);
}