#include <unordered_map>
#include "SDL.h"
#include "engine/Engine.h"
#include "engine/SnapQueue.h"
#include "engine/Physics.h"
#include "engine/Clock.h"

void trippin::Engine::add(Object *obj) {
    grid.add(obj);
    auto &vec = obj->platform ? platforms : objects;
    vec.push_back(obj);
}

void trippin::Engine::setGridSize(Point<int> gridSize, Point<int> cellSize) {
    grid.setSize(gridSize, cellSize);
}

void trippin::Engine::beforeTick(Clock clock) {
    for (auto obj : objects) {
        obj->beforeTick(clock);
    }
}

void trippin::Engine::afterTick(Clock clock) {
    for (auto obj : objects) {
        obj->afterTick(clock);
    }
}

void trippin::Engine::tick(Clock clock) {
    beforeTick(clock);
    applyMotion();
    snapObjects();
    applyPhysics();
    afterTick(clock);
}

void trippin::Engine::applyMotion() {
    for (auto obj : objects) {
        obj->applyMotion();
        grid.update(obj);
    }
}

void trippin::Engine::snapObjects(Partition &partition) {
    // early exit if no work to do
    if (partition.objects.empty()) {
        return;
    }

    std::unordered_map<Object *, Sides> collisions{};
    SnapQueue q{};

    // Load all objects into queue
    for (auto obj : partition.platforms) {
        q.push(obj);
    }
    for (auto obj : partition.objects) {
        q.push(obj);
    }

    // Traverse objects in priority order where priority is determined by platform contacts.
    // Each object acts like a platform once. All non-platform, overlapping neighbor objects are snapped.
    while (!q.empty()) {
        auto plat = q.pop();
        for (auto kin : partition.objects) {
            if (plat != kin && !kin->platform) {
                auto overlap = kin->roundedBox.intersect(plat->roundedBox);
                if (overlap) {
                    snapTo(*kin, *plat, overlap, collisions[kin]);
                }
                auto collision = kin->roundedBox.collision(plat->roundedBox);
                if (collision) {
                    auto c = collisions[kin] |= collision;
                    q.push(kin, c);
                }
            }
        }
    }
}

void trippin::Engine::snapObjects() {
    for (auto &partition : grid.partitions) {
        snapObjects(partition);
    }
}

void trippin::Engine::snapTo(Object &obj, const Object &p, const Rect<int> &overlap, const Sides &previousCollisions) {
    auto x = 0.0;
    auto y = 0.0;

    // examine horizontal overlap - the overlap rect must align with left-right sides of the two objects
    // this check excludes interior collisions, for example a tally, skinny object that drops vertically in the
    // middle of a wide platform
    if (obj.roundedBox.leftAlignedWith(overlap) && p.roundedBox.rightAlignedWith(overlap)) {
        x = overlap.w;
    } else if (obj.roundedBox.rightAlignedWith(overlap) && p.roundedBox.leftAlignedWith(overlap)) {
        x = -overlap.w;
    }

    // examine vertical overlap - the overlap rect must align with top-bottom sides of the two objects
    // this check excludes interior collisions, for example a short, wide object that move laterally in the
    // middle of a tall platform
    if (obj.roundedBox.topAlignedWith(overlap) && p.roundedBox.bottomAlignedWith(overlap)) {
        y = overlap.h;
    } else if (obj.roundedBox.bottomAlignedWith(overlap) && p.roundedBox.topAlignedWith(overlap)) {
        y = -overlap.h;
    }

    bool updated = false;

    // use smallest displacement
    if (x != 0 && (y == 0 || (std::abs(x) < std::abs(y)))) {
        // dont snap back over previous overlap - oldest snap wins
        if ((x > 0 && !previousCollisions.testRight()) || (x < 0 && !previousCollisions.testLeft())) {
            obj.position.x += x;
            updated = true;
        }
    } else if (y != 0) {
        // same as above
        if ((y > 0 && !previousCollisions.testBottom()) || (y < 0 && !previousCollisions.testTop())) {
            obj.position.y += y;
            updated = true;
        }
    }

    if (updated) {
        obj.updateRounded();
    }
}

void trippin::Engine::applyPhysics() {
    for (auto &partition : grid.partitions) {
        for (auto platform : partition.platforms) {
            for (auto object : partition.objects) {
                auto collision = object->roundedBox.collision(platform->roundedBox);
                if (collision) {
                    applyPlatformCollision(*object, *platform, collision);
                }
            }
        }

        for (int i = 0; i < partition.objects.size(); i++) {
            auto a = partition.objects[i];
            for (int j = i + 1; j < partition.objects.size(); j++) {
                auto b = partition.objects[j];
                auto collision = a->roundedBox.collision(b->roundedBox);
                if (collision) {
                    applyObjectCollision(*a, *b, collision);
                }
            }
        }
    }
}

void trippin::Engine::applyPlatformCollision(Object &object, Object &platform, const Sides &collision) {
    auto type = object.platformCollisionType.isPresent()
                ? object.platformCollisionType.get()
                : platformCollisionType;
    object.onPlatformCollision(platform, collision);
    if (type == PlatformCollisionType::absorbant) {
        absorbantCollision(object, platform, collision);
    } else {
        reflectiveCollision(object, platform, collision);
    }
}

void trippin::Engine::absorbantCollision(Object &obj, Object &p, const Sides &collision) {
    if ((collision.testLeft() && obj.velocity.x < 0) || (collision.testRight() && obj.velocity.x > 0)) {
        obj.velocity.x = 0;
    }
    if ((collision.testTop() && obj.velocity.y < 0) || (collision.testBottom() && obj.velocity.y > 0)) {
        obj.velocity.y = 0;
    }
    obj.platformCollisions |= collision;
}

void trippin::Engine::reflectiveCollision(Object &object, Object &platform, const Sides &collision) {
    if ((collision.testLeft() && object.velocity.x < 0) || (collision.testRight() && object.velocity.x > 0)) {
        object.velocity.x *= -1;
    }
    if ((collision.testTop() && object.velocity.y < 0) || (collision.testBottom() && object.velocity.y > 0)) {
        object.velocity.y *= -1;
    }
}

void trippin::Engine::applyObjectCollision(Object &left, Object &right, const Sides &collision) {
    left.objectCollisions |= collision;
    right.objectCollisions |= collision.flip();
    left.onObjectCollision(right, collision);
    right.onObjectCollision(left, collision.flip());

    if (objectCollisionType == ObjectCollisionType::elastic1D) {
        elasticCollision1D(left, right, collision);
    } else if (objectCollisionType == ObjectCollisionType::elastic2D) {
        elasticCollision2D(left, right, collision);
    } else {
        inelasticCollision2D(left, right, collision);
    }
}

void trippin::Engine::elasticCollision1D(Object &obj, Object &p, const Sides &collision) {
    if (rationalHorizontalCollision(obj, p, collision)) {
        auto vels = trippin::elasticCollision1D(obj.velocity.x, p.velocity.x, obj.mass, p.mass);
        obj.velocity.x = vels.first;
        p.velocity.x = vels.second;
    }
    if (rationalVerticalCollision(obj, p, collision)) {
        auto vels = trippin::elasticCollision1D(obj.velocity.y, p.velocity.y, obj.mass, p.mass);
        obj.velocity.y = vels.first;
        p.velocity.y = vels.second;
    }
}

void trippin::Engine::elasticCollision2D(Object &obj, Object &p, const Sides &collision) {
    if (rationalCollision(obj, p, collision)) {
        auto pair = trippin::elasticCollision2D(obj.velocity, p.velocity, obj.center, p.center, obj.mass, p.mass);
        obj.velocity = pair.first;
        p.velocity = pair.second;
    }
}

void trippin::Engine::inelasticCollision2D(Object &obj, Object &p, const Sides &collision) {
    if (rationalCollision(obj, p, collision)) {
        auto pair = trippin::inelasticCollision2D(
                obj.velocity, p.velocity,
                obj.center, p.center,
                obj.mass, p.mass,
                restitutionCoefficient);
        obj.velocity = pair.first;
        p.velocity = pair.second;
    }
}

bool trippin::Engine::rationalCollision(const Object &obj, const Object &p, const Sides &collision) {
    return rationalHorizontalCollision(obj, p, collision) || rationalVerticalCollision(obj, p, collision);
}

bool trippin::Engine::rationalHorizontalCollision(const Object &obj, const Object &p, const Sides &collision) {
    auto &ov = obj.velocity;
    auto &pv = p.velocity;
    auto leftCollision = collision.testLeft() && ((ov.x < 0 && ov.x < pv.x) || (pv.x > 0 && pv.x > ov.x));
    bool rightCollision = collision.testRight() && ((ov.x > 0 && ov.x > pv.x) || (pv.x < 0 && pv.x < ov.x));
    return leftCollision || rightCollision;
}

bool trippin::Engine::rationalVerticalCollision(const Object &obj, const Object &p, const Sides &collision) {
    auto &ov = obj.velocity;
    auto &pv = p.velocity;
    auto topCollision = collision.testTop() && ((ov.y < 0 && ov.y < pv.y) || (pv.y > 0 && pv.y > ov.y));
    auto bottomCollision = collision.testBottom() && ((ov.y > 0 && ov.y > pv.y) || (pv.y < 0 && pv.y < ov.y));
    return topCollision || bottomCollision;
}

void trippin::Engine::setPlatformCollisionType(PlatformCollisionType t) {
    platformCollisionType = t;
}

void trippin::Engine::setObjectCollisionType(ObjectCollisionType t) {
    objectCollisionType = t;
}

void trippin::Engine::setRestitutionCoefficient(double r) {
    restitutionCoefficient = r;
}

void trippin::Engine::setTickPeriod(int tp) {
    tickPeriod = tp;
}

int run(void *data) {
    auto engine = (trippin::Engine *) data;
    engine->runEngineLoop();
    return 0;
}

void trippin::Engine::runEngineLoop() {
    Clock clock;
    clock.setTickPeriod(tickPeriod);
    while (!stopped) {
        if (!paused) {
            tick(clock);
        }
        clock.next();
    }
}

void trippin::Engine::start() {
    thread = SDL_CreateThread(run, "Engine Thread", (void *) this);
}

void trippin::Engine::pause() {
    paused = true;
}

void trippin::Engine::stop() {
    stopped = true;
}
