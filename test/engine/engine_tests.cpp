#include "catch2/catch.hpp"
#include "engine/Engine.h"

TEST_CASE("Engine object platform collision", "[engine]")
{
    trippin::Engine engine;
    engine.setPlatformCollisionType(trippin::PlatformCollisionType::absorbant);
    engine.setObjectCollisionType(trippin::ObjectCollisionType::elastic1D);

    trippin::Object a;
    a.setPlatform(true);
    a.setSize({4, 4});
    a.setPosition({0, 0});

    trippin::Object b;
    b.setPlatform(false);
    b.setSize({4, 4});
    b.setPosition({0, 3});

    engine.add(&a);
    engine.add(&b);

    engine.tick();

    REQUIRE(a.getRoundedBox() == trippin::Rect<int>{0, 0, 4, 4});
    REQUIRE(b.getRoundedBox() == trippin::Rect<int>{0, 4, 4, 4});

    REQUIRE(b.getPlatformCollisions().testTop());
}

TEST_CASE("Engine object snap reaction", "[engine]")
{
    trippin::Engine engine;
    engine.setPlatformCollisionType(trippin::PlatformCollisionType::absorbant);
    engine.setObjectCollisionType(trippin::ObjectCollisionType::elastic1D);

    trippin::Object a;
    a.setPlatform(true);
    a.setSize({4, 4});
    a.setPosition({0, 0});

    trippin::Object b;
    b.setPlatform(false);
    b.setSize({4, 4});
    b.setPosition({3, 0});

    trippin::Object c;
    c.setPlatform(false);
    c.setSize({4, 4});
    c.setPosition({6, 0});

    engine.add(&a);
    engine.add(&b);
    engine.add(&c);

    engine.tick();

    REQUIRE(a.getRoundedBox() == trippin::Rect<int>{0, 0, 4, 4});
    REQUIRE(b.getRoundedBox() == trippin::Rect<int>{4, 0, 4, 4});
    REQUIRE(c.getRoundedBox() == trippin::Rect<int>{8, 0, 4, 4});

    REQUIRE(b.getPlatformCollisions().testLeft());
    REQUIRE(c.getPlatformCollisions() == trippin::Sides{});
}

TEST_CASE("Engine object 1D collision", "[engine]")
{
    trippin::Engine engine;
    engine.setPlatformCollisionType(trippin::PlatformCollisionType::absorbant);
    engine.setObjectCollisionType(trippin::ObjectCollisionType::elastic1D);

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

    engine.add(&a);
    engine.add(&b);

    engine.tick();

    REQUIRE(a.getPosition().x == 1);
    REQUIRE(b.getPosition().x == 11);
    REQUIRE(a.getVelocity().x == -1);
    REQUIRE(b.getVelocity().x == 1);

    engine.tick();

    REQUIRE(a.getPosition().x == 0);
    REQUIRE(b.getPosition().x == 12);
    REQUIRE(a.getVelocity().x == -1);
    REQUIRE(b.getVelocity().x == 1);
}

TEST_CASE("Engine object 1D fractional collision no oscillation", "[engine]")
{
    trippin::Engine engine;
    engine.setPlatformCollisionType(trippin::PlatformCollisionType::absorbant);
    engine.setObjectCollisionType(trippin::ObjectCollisionType::elastic1D);

    trippin::Object a;
    a.setPlatform(false);
    a.setSize({10, 10});
    a.setPosition({0, 0});
    a.setVelocity({0.1, 0});
    a.setMass(1.0);

    trippin::Object b;
    b.setPlatform(false);
    b.setSize({10, 10});
    b.setPosition({10.2, 0});
    b.setVelocity({-0.1, 0});
    b.setMass(1.0);

    engine.add(&a);
    engine.add(&b);

    engine.tick();

    REQUIRE(a.getPosition().x == 0.1);
    REQUIRE(b.getPosition().x == 10.1);
    REQUIRE(a.getVelocity().x == -0.1);
    REQUIRE(b.getVelocity().x == 0.1);

    engine.tick();

    REQUIRE(a.getPosition().x == 0);
    REQUIRE(b.getPosition().x == 10.2);
    REQUIRE(a.getVelocity().x == -0.1);
    REQUIRE(b.getVelocity().x == 0.1);
}

TEST_CASE("Engine object 1D fractional collision no irrational2", "[engine]")
{
    trippin::Engine engine;
    engine.setPlatformCollisionType(trippin::PlatformCollisionType::absorbant);
    engine.setObjectCollisionType(trippin::ObjectCollisionType::elastic1D);

    trippin::Object a;
    a.setPlatform(true);
    a.setSize({100, 10});
    a.setPosition({0, 100});

    trippin::Object b;
    b.setPlatform(false);
    b.setSize({10, 10});
    b.setPosition({0, 91});
    b.setVelocity({0, 1});
    b.setMass(1.0);

    trippin::Object c;
    c.setPlatform(false);
    c.setSize({10, 10});
    c.setPosition({0, 81});
    c.setVelocity({0, -1});
    c.setMass(1.0);

    engine.add(&a);
    engine.add(&b);
    engine.add(&c);

    engine.tick();

    // object b snaps up by 1 unit, causing collision
    REQUIRE(b.getPosition().y == 90);
    REQUIRE(c.getPosition().y == 80);

    // objects collide in an irrational way due to snapping
    // no velocity collision reaction expected
    REQUIRE(b.getPlatformCollisions().testBottom());
    REQUIRE(b.getVelocity().y == 0);
    REQUIRE(c.getVelocity().x == 0);
    REQUIRE(c.getVelocity().y == -1);
}