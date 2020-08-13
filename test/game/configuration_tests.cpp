#include "catch2/catch.hpp"
#include "game/Configuration.h"

TEST_CASE("Load test configuration", "[configuration]")
{
    trippin::Configuration config{};
    config.load("test");
}