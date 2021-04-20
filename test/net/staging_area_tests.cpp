#include "catch2/catch.hpp"
#include "net/StagingArea.h"

TEST_CASE("Combine distinct", "[staging]")
{
    trippin::StagingArea sa;
    sa.addScore({12345, 123, "ABCDE"});
    sa.setTodayScores({{23456, 456, "FGHIJ"}});
    auto scores = sa.getTodayScores(10);
    REQUIRE(scores.size() == 2);
    REQUIRE(scores[0].name == "FGHIJ");
    REQUIRE(scores[1].name == "ABCDE");
}

TEST_CASE("Combine collision", "[staging]")
{
    trippin::StagingArea sa;
    trippin::Score ds{12345, 123, "ABCDE"};
    sa.addScore(ds);
    sa.setTodayScores({ds});
    auto scores = sa.getTodayScores(10);
    REQUIRE(scores.size() == 1);
    REQUIRE(scores[0].name == "ABCDE");
}