#include "catch2/catch.hpp"
#include "net/StagingArea.h"

TEST_CASE("Combine distinct", "[staging]")
{
    trippin::StagingArea sa;
    sa.addScore({"abc123", 1, 100, "ABC", {}});
    sa.setTodayScores({{"def456", 1, 200, "DEF", {}}});
    auto scores = sa.getTodayScores(10);
    REQUIRE(scores.size() == 2);
    REQUIRE(scores[0].name == "DEF");
    REQUIRE(scores[1].name == "ABC");
}

TEST_CASE("Combine collision", "[staging]")
{
    trippin::StagingArea sa;
    trippin::Score ds{"abc123", 1, 100, "ABC", {}};
    sa.addScore(ds);
    sa.setTodayScores({ds});
    auto scores = sa.getTodayScores(10);
    REQUIRE(scores.size() == 1);
    REQUIRE(scores[0].name == "ABC");
}