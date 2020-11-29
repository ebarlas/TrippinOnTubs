#include "catch2/catch.hpp"
#include "engine/Rect.h"

TEST_CASE("Rect initialization", "[rect]") {
    REQUIRE(trippin::Rect<int>{1, 2, 3, 4}.x == 1);
}

TEST_CASE("Rect intersect none", "[rect]") {
    trippin::Rect<int> a{0, 0, 4, 4};
    trippin::Rect<int> b{4, 4, 4, 4};
    REQUIRE(!a.intersect(b));
}

TEST_CASE("Rect intersect top bottom", "[rect]") {
    trippin::Rect<int> a{0, 0, 4, 4};
    trippin::Rect<int> b{0, 3, 4, 4};
    trippin::Rect<int> c = a.intersect(b);
    REQUIRE(c);
    REQUIRE(a.bottomAlignedWith(c));
    REQUIRE(b.topAlignedWith(c));
}

TEST_CASE("Rect intersect left right", "[rect]") {
    trippin::Rect<int> a{0, 0, 4, 4};
    trippin::Rect<int> b{3, 0, 4, 4};
    trippin::Rect<int> c = a.intersect(b);
    REQUIRE(c);
    REQUIRE(a.rightAlignedWith(c));
    REQUIRE(b.leftAlignedWith(c));
}