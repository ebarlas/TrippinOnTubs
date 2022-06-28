#include "catch2/catch.hpp"
#include "game/Shake.h"
#include <iostream>

TEST_CASE("Shake test", "[shake]")
{
    trippin::Shake shake;
    shake.init(2, 10);
    REQUIRE(shake.amplitude() == 0.0);
    int ticks = 0;
    shake.start(0);
    for (int i = 0; i < 10; i++) {
        auto amp = shake.amplitude();
        REQUIRE(amp <= 1.0);
        REQUIRE(amp >= -1.0);
        ticks++;
        shake.update(ticks);
    }
    REQUIRE(shake.amplitude() == 0.0);
}