#include <iostream>
#include <iomanip>
#include "catch2/catch.hpp"
#include "engine/Engine.h"
#include "engine/Collisions.h"

TEST_CASE("Engine object platform collision", "[engine]")
{
    trippin::Engine engine;

    trippin::Object a;
    a.platform = true;
    a.size = {4, 4};
    a.position = {0, 0};
    a.syncPositions();

    trippin::Object b;
    b.platform = false;
    b.size = {4, 4};
    b.position = {0, 3};
    b.syncPositions();

    engine.add(&a);
    engine.add(&b);

    engine.tick(1);

    REQUIRE(a.roundedBox == trippin::Rect<int>{0, 0, 4, 4});
    REQUIRE(b.roundedBox == trippin::Rect<int>{0, 4, 4, 4});

    REQUIRE(b.platformCollisions.testTop());
}

TEST_CASE("Engine object platform interior", "[engine]")
{
    trippin::Engine engine;
    engine.setObjectCollision(trippin::onElasticCollision1D);

    trippin::Object a;
    a.platform = true;
    a.size = {4, 12};
    a.position = {2, 2};
    a.syncPositions();

    trippin::Object b;
    b.platform = false;
    b.size = {9, 2};
    b.position = {3, 6};
    b.syncPositions();

    engine.add(&a);
    engine.add(&b);

    engine.tick(1);

    REQUIRE(a.roundedBox == trippin::Rect<int>{2, 2, 4, 12});
    REQUIRE(b.roundedBox == trippin::Rect<int>{6, 6, 9, 2});
}

TEST_CASE("Engine object snap reaction", "[engine]")
{
    trippin::Engine engine;
    engine.setObjectCollision(trippin::onElasticCollision1D);

    trippin::Object a;
    a.platform = true;
    a.size = {4, 4};
    a.position = {0, 0};
    a.syncPositions();

    trippin::Object b;
    b.platform = false;
    b.size = {4, 4};
    b.position = {3, 0};
    b.syncPositions();

    trippin::Object c;
    c.platform = false;
    c.size = {4, 4};
    c.position = {6, 0};
    c.syncPositions();

    engine.add(&a);
    engine.add(&b);
    engine.add(&c);

    engine.tick(1);

    REQUIRE(a.roundedBox == trippin::Rect<int>{0, 0, 4, 4});
    REQUIRE(b.roundedBox == trippin::Rect<int>{4, 0, 4, 4});
    REQUIRE(c.roundedBox == trippin::Rect<int>{8, 0, 4, 4});

    REQUIRE(b.platformCollisions.testLeft());
    REQUIRE(c.platformCollisions == trippin::Sides{});
}

TEST_CASE("Engine object snap reaction circle", "[engine]")
{
    trippin::Engine engine;
    engine.setObjectCollision(trippin::onElasticCollision1D);

    trippin::Object a;
    a.id = 1;
    a.platform = true;
    a.size = {4, 4};
    a.position = {9, 5};
    a.syncPositions();

    trippin::Object b;
    b.id = 2;
    b.platform = false;
    b.size = {4, 4};
    b.position = {11, 8};
    b.syncPositions();

    trippin::Object c;
    c.id = 3;
    c.platform = false;
    c.size = {7, 4};
    c.position = {6, 10};
    c.syncPositions();

    trippin::Object d;
    d.id = 4;
    d.platform = false;
    d.size = {6, 8};
    d.position = {1, 4};
    d.syncPositions();

    trippin::Object e;
    e.id = 5;
    e.platform = false;
    e.size = {6, 6};
    e.position = {2, 2};
    e.syncPositions();

    engine.add(&a);
    engine.add(&b);
    engine.add(&c);
    engine.add(&d);
    engine.add(&e);

    engine.tick(1);

    REQUIRE(a.roundedBox == trippin::Rect<int>{9, 5, 4, 4});
    REQUIRE(b.roundedBox == trippin::Rect<int>{11, 9, 4, 4});
    REQUIRE(c.roundedBox == trippin::Rect<int>{4, 10, 7, 4});
    REQUIRE(d.roundedBox == trippin::Rect<int>{1, 2, 6, 8});
    REQUIRE(e.roundedBox == trippin::Rect<int>{7, 2, 6, 6});
}

TEST_CASE("Engine object 1D collision", "[engine]")
{
    trippin::Engine engine;
    engine.setObjectCollision(trippin::onElasticCollision1D);

    trippin::Object a;
    a.platform = false;
    a.size = {10, 10};
    a.position = {0, 0};
    a.velocity = {1, 0};
    a.mass = 1.0;
    a.syncPositions();

    trippin::Object b;
    b.platform = false;
    b.size = {10, 10};
    b.position = {12, 0};
    b.velocity = {-1, 0};
    b.mass = 1.0;
    b.syncPositions();

    engine.add(&a);
    engine.add(&b);

    engine.tick(1);

    REQUIRE(a.position.x == 1);
    REQUIRE(b.position.x == 11);
    REQUIRE(a.velocity.x == -1);
    REQUIRE(b.velocity.x == 1);

    engine.tick(1);

    REQUIRE(a.position.x == 0);
    REQUIRE(b.position.x == 12);
    REQUIRE(a.velocity.x == -1);
    REQUIRE(b.velocity.x == 1);
}

TEST_CASE("Engine object 1D fractional collision no oscillation", "[engine]")
{
    trippin::Engine engine;
    engine.setObjectCollision(trippin::onElasticCollision1D);

    trippin::Object a;
    a.platform = false;
    a.size = {10, 10};
    a.position = {0, 0};
    a.velocity = {0.1, 0};
    a.mass = 1.0;
    a.syncPositions();

    trippin::Object b;
    b.platform = false;
    b.size = {10, 10};
    b.position = {10.2, 0};
    b.velocity = {-0.1, 0};
    b.mass = 1.0;
    b.syncPositions();

    engine.add(&a);
    engine.add(&b);

    engine.tick(1);

    REQUIRE(a.position.x == 0.1);
    REQUIRE(b.position.x == 10.1);
    REQUIRE(a.velocity.x == -0.1);
    REQUIRE(b.velocity.x == 0.1);

    engine.tick(1);

    REQUIRE(a.position.x == 0);
    REQUIRE(b.position.x == 10.2);
    REQUIRE(a.velocity.x == -0.1);
    REQUIRE(b.velocity.x == 0.1);

    engine.tick(1);

    REQUIRE(a.position.x == -0.1);
    REQUIRE(b.position.x == 10.0 + 0.1 + 0.1 + 0.1);
    REQUIRE(a.velocity.x == -0.1);
    REQUIRE(b.velocity.x == 0.1);
}

TEST_CASE("Engine object 1D fractional collision no irrational", "[engine]")
{
    trippin::Engine engine;
    engine.setObjectCollision(trippin::onElasticCollision1D);

    trippin::Object a;
    a.platform = true;
    a.size = {100, 10};
    a.position = {0, 100};
    a.syncPositions();

    trippin::Object b;
    b.platform = false;
    b.size = {10, 10};
    b.position = {0, 91};
    b.velocity = {0, 1};
    b.mass = 1.0;
    b.syncPositions();

    trippin::Object c;
    c.platform = false;
    c.size = {10, 10};
    c.position = {0, 81};
    c.velocity = {0, -1};
    c.mass = 1.0;
    c.syncPositions();

    engine.add(&a);
    engine.add(&b);
    engine.add(&c);

    engine.tick(1);

    // object b snaps up by 1 unit, causing collision
    REQUIRE(b.position.y == 90);
    REQUIRE(c.position.y == 80);

    // objects collide in an irrational way due to snapping
    // no velocity collision reaction expected
    REQUIRE(b.platformCollisions.testBottom());
    REQUIRE(b.velocity.y == 0);
    REQUIRE(c.velocity.x == 0);
    REQUIRE(c.velocity.y == -1);
}

std::ostream& operator<<(std::ostream& os, const trippin::Object& obj) {
    return os << "obj[id=" << obj.id << ", pos=(" << obj.position << "), vel=(" << obj.velocity << ")]";
}

// This test demonstrates how the elastic 2D collision physics leads to
// odd oscillation behavior when boxes collide using center points
// Update: using the Plasma Physics function seems to have fixed the oscillation problem.
// Now the objects stick together. Shrug.
TEST_CASE("Velocity oscillation observed in enclosed demo app", "[engine]")
{
    trippin::Engine engine;
    engine.setPlatformCollision(trippin::onReflectiveCollisionDefault);
    engine.setObjectCollision(trippin::onElasticCollision2D);

    trippin::Object a;
    a.id = 11;
    a.mass = 9030;
    a.platform = false;
    a.size = {129, 70};
    a.position = {576.40, 494.87};
    a.velocity = {1.79, -1.05};
    a.syncPositions();

    trippin::Object b;
    b.id = 24;
    b.mass = 8280;
    b.platform = false;
    b.size = {138, 60};
    b.position = {476.05, 435.11};
    b.velocity = {-0.33, 1.00};
    b.syncPositions();

    engine.add(&a);
    engine.add(&b);

    std::cout << std::setprecision(2) << std::fixed << a << ", " << b << std::endl;

    for (int i=1; i<=5; i++) {
        engine.tick(i);
        std::cout << a << ", " << b << std::endl;
    }
}