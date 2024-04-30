#include "catch2/catch.hpp"
#include "net/StagingArea.h"

TEST_CASE("Add score", "[staging]")
{
    trippin::Transport t{"localhost", 8080, 1, 10};
    trippin::StagingArea sa{t};
    sa.addScore({"abc123", 1, 100, "ABC", {}});
    auto scores = sa.getTodayScores(10);
    REQUIRE(scores.size() == 1);
    REQUIRE(scores[0].name == "ABC");
}