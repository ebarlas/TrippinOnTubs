#include "catch2/catch.hpp"
#include "engine/CollisionTable.h"

TEST_CASE("Collision table lifecycle", "[collision table]") {
    trippin::Object a;
    a.setId(1);
    a.setVelocity({1, 2});

    trippin::Object b;
    b.setId(2);
    b.setVelocity({3, 4});

    trippin::CollisionTable ct;

    REQUIRE(!ct.test(a, b));
    REQUIRE(!ct.test(b, a));

    ct.add(a, b);
    REQUIRE(!ct.test(a, b));
    REQUIRE(!ct.test(b, a));

    ct.rotate();
    REQUIRE(ct.test(a, b));
    REQUIRE(ct.test(b, a));

    trippin::Object t;
    a.setId(1);
    a.setVelocity({2, 3});
    REQUIRE(!ct.test(t, b));
}