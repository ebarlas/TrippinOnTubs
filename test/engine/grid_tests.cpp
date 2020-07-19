#include "catch2/catch.hpp"
#include "engine/Grid.h"

TEST_CASE("Grid layout", "[grid]") {
    trippin::Grid grid;
    grid.setSize({2, 2}, {10, 10});
    REQUIRE(grid.partitions.size() == 5);

    trippin::Object obj{};
    obj.setPosition({2, 2});
    obj.setSize({5, 5});
    grid.add(&obj);
    REQUIRE(grid.partitions[0].objects.size() == 1);
    REQUIRE(grid.partitions[1].objects.empty());

    obj.setPosition({5, 5});
    grid.update(&obj);
    REQUIRE(grid.partitions[0].objects.size() == 1);
    REQUIRE(grid.partitions[1].objects.size() == 1);
    REQUIRE(grid.partitions[2].objects.size() == 1);

    obj.setPosition({15, 15});
    grid.update(&obj);
    REQUIRE(grid.partitions[0].objects.empty());
    REQUIRE(grid.partitions[1].objects.empty());
    REQUIRE(grid.partitions[2].objects.empty());
    REQUIRE(grid.partitions[3].objects.size() == 1);
    REQUIRE(grid.partitions[4].objects.size() == 1);

    obj.setPosition({25, 25});
    grid.update(&obj);
    REQUIRE(grid.partitions[0].objects.empty());
    REQUIRE(grid.partitions[1].objects.empty());
    REQUIRE(grid.partitions[2].objects.empty());
    REQUIRE(grid.partitions[3].objects.empty());
    REQUIRE(grid.partitions[4].objects.size() == 1);
}