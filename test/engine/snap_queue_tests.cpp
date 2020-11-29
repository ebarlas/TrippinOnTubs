#include "catch2/catch.hpp"
#include "engine/SnapQueue.h"

TEST_CASE("Snap queue platform object", "[snap queue]") {
    trippin::Object p{};
    p.platform = true;

    trippin::Object obj{};
    obj.platform = false;

    trippin::SnapQueue q;
    q.push(&obj);
    q.push(&p);
    REQUIRE(q.pop() == &p);
    REQUIRE(q.pop() == &obj);
    REQUIRE(q.empty());
}

TEST_CASE("Snap queue duplicate discard add", "[snap queue]") {
    trippin::Object obj{};
    obj.platform = false;

    trippin::SnapQueue q;
    q.push(&obj);
    REQUIRE(!q.empty());
    q.pop();
    REQUIRE(q.empty());
    q.push(&obj, {});
    REQUIRE(q.empty());
}

TEST_CASE("Snap queue duplicate discard queued", "[snap queue]") {
    trippin::Object p{};
    p.platform = true;

    trippin::Object obj{};
    obj.platform = false;

    trippin::SnapQueue q;
    q.push(&p);
    q.push(&p, {});
    q.push(&obj);
    REQUIRE(q.pop() == &p);
    REQUIRE(q.pop() == &obj);
    REQUIRE(q.empty());
}