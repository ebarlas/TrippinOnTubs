#include "catch2/catch.hpp"
#include "net/StagingArea.h"

TEST_CASE("Combine distinct", "[staging]")
{
    trippin::StagingArea sa;
    sa.addScore({12345, "ABCDE", "abc123"});
    sa.setTodayScores({{23456, "FGHIJ", "def456"}});
    auto scores = sa.getTodayScores();
    REQUIRE(scores.size() == 2);
    REQUIRE(scores[0].name == "FGHIJ");
    REQUIRE(scores[1].name == "ABCDE");
}

TEST_CASE("Combine collision", "[staging]")
{
    trippin::StagingArea sa;
    trippin::Score ds{12345, "ABCDE", "abc123"};
    sa.addScore(ds);
    sa.setTodayScores({ds});
    auto scores = sa.getTodayScores();
    REQUIRE(scores.size() == 1);
    REQUIRE(scores[0].name == "ABCDE");
}