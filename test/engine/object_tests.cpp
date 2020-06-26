#include "catch2/catch.hpp"
#include "engine/object.h"

TEST_CASE("Object snap", "[object]") {
    trippin::Object a;
    a.setPlatform(true);
    a.setSize({4, 4});
    a.setPosition({0, 0});

    trippin::Object b;
    b.setPlatform(false);
    b.setSize({4, 4});
    b.setPosition({0, 3});

    auto overlap = a.getRoundedBox().intersect(b.getRoundedBox());
    REQUIRE(overlap.w == 4);
    REQUIRE(overlap.h == 1);

    b.snapTo(a, overlap, {});
    REQUIRE(a.getRoundedBox() == trippin::Rect<int>{0, 4, 4, 4});
}

TEST_CASE("Object snap reaction", "[object]") {
    trippin::Object a;
    a.setPlatform(true);
    a.setSize({4, 4});
    a.setPosition({0, 0});

    trippin::Object b;
    b.setPlatform(false);
    b.setSize({4, 4});
    b.setPosition({3, 3});

    trippin::Object c;
    c.setPlatform(false);
    c.setSize({4, 4});
    c.setPosition({6, 6});

    std::vector<trippin::Object *> objects{&a, &b, &c};
    trippin::Object::snapObjects(objects);

    REQUIRE(b.getRoundedBox() == trippin::Rect<int>{4, 4, 4, 4});
    REQUIRE(c.getRoundedBox() == trippin::Rect<int>{8, 8, 4, 4});
}

TEST_CASE("Object platform collision", "[object]") {
    trippin::Object a;
    a.setPlatform(true);
    a.setSize({100, 10});
    a.setPosition({0, 10});

    trippin::Object b;
    b.setPlatform(false);
    b.setSize({6, 6});
    b.setPosition({0, 0});
    b.setVelocity({0, 3});

    auto fn = [](trippin::Object &a, trippin::Object &b, const trippin::Sides &collision) {
        return a.absorbantCollision(b, collision);
    };

    std::vector<trippin::Object *> objects{&a, &b};

    auto tick = [&objects, &fn]() {
        trippin::Object::applyMotion(objects);
        trippin::Object::snapObjects(objects);
        trippin::Object::applyPhysics(objects, fn, fn);
    };

    tick();
    REQUIRE(b.getPosition().y == 3);

    tick();
    REQUIRE(b.getPosition().y == 4);
}

TEST_CASE("Object 1D elastic collision", "[object]") {
    trippin::Object a;
    a.setPlatform(false);
    a.setSize({10, 10});
    a.setPosition({0, 0});
    a.setVelocity({1, 0});
    a.setMass(1.0);

    trippin::Object b;
    b.setPlatform(false);
    b.setSize({10, 10});
    b.setPosition({12, 0});
    b.setVelocity({-1, 0});
    b.setMass(1.0);

    auto fn = [](trippin::Object &a, trippin::Object &b, const trippin::Sides &collision) {
        return a.elasticCollision1D(b, collision);
    };

    std::vector<trippin::Object *> objects{&a, &b};
    trippin::Object::applyMotion(objects);
    trippin::Object::snapObjects(objects);
    trippin::Object::applyPhysics(objects, fn, fn);

    REQUIRE(a.getPosition().x == 1);
    REQUIRE(b.getPosition().x == 11);
    REQUIRE(a.getVelocity().x == -1);
    REQUIRE(b.getVelocity().x == 1);
}