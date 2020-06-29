#include "catch2/catch.hpp"
#include "sprite/SpriteMetadata.h"

TEST_CASE("Sprite metadata load", "[sprite]")
{
    trippin::SpriteMetadata sm{};
    sm.load("goggin");
    REQUIRE(sm.getFrames() == 8);
    REQUIRE(sm.getDuration() == 80);
}