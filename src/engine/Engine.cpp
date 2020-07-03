#include "Engine.h"

void trippin::Engine::add(trippin::Object *object) {
    if (object->platform) {
        platforms.push_back(object);
    } else {
        objects.push_back(object);
    }
}

void trippin::Engine::tick() {
    transferCollisions();
    applyMotion();
    snapObjects();
    applyPhysics();
}

void trippin::Engine::transferCollisions() {
    prevCollisions = std::move(nextCollisions);
    nextCollisions.clear();
}

void trippin::Engine::applyMotion() {
    for (auto obj : objects) {
        obj->applyMotion();
    }
}

void trippin::Engine::snapObjects() {
    auto compare = [](const std::pair<Object *, Sides> &left, const std::pair<Object *, Sides> &right) {
        // 1. platforms are highest priority
        if (left.first->platform)
            return false;
        if (right.first->platform)
            return true;

        // 2. non-platforms with most collision sides is next highest priority
        auto lcnt = left.second.count();
        auto rcnt = right.second.count();
        if (lcnt != rcnt)
            return lcnt < rcnt;

        // 3. non-platforms with highest y position (lower on screen) is next highest priority
        return left.first->position.y < right.first->position.y;
    };

    Queue<Object *, Sides, decltype(compare)> q(compare);

    // 1. initialize queue with all objects
    for (auto obj : platforms) {
        q.push(obj);
    }
    for (auto obj : objects) {
        q.push(obj);
    }

    // 2. traverse objects in priority order where priority is determined by platform contacts
    while (!q.empty()) {
        auto plat = q.pop();
        for (auto kin : objects) {
            if (plat != kin && !kin->platform) {
                auto overlap = kin->roundedBox.intersect(plat->roundedBox);
                if (overlap) {
                    snapTo(*kin, *plat, overlap, q.material(kin));
                }
                auto collision = kin->roundedBox.collision(plat->roundedBox);
                if (collision) {
                    q.push(kin, collision);
                }
            }
        }
    }
}

void trippin::Engine::snapTo(
        Object &obj,
        const Object &p,
        const trippin::Rect<int> &overlap,
        const Sides &previousCollisions) {
    auto x = 0.0;
    auto y = 0.0;

    // examine horizontal overlap
    if (obj.roundedBox.leftAlignedWith(overlap)) {
        x = overlap.w;
    } else if (obj.roundedBox.rightAlignedWith(overlap)) {
        x = -overlap.w;
    }

    // examine vertical overlap
    if (obj.roundedBox.topAlignedWith(overlap)) {
        y = overlap.h;
    } else if (obj.roundedBox.bottomAlignedWith(overlap)) {
        y = -overlap.h;
    }

    bool updated = false;

    // use smallest displacement
    if (x != 0 && std::abs(x) < std::abs(y)) {
        // dont snap back over previous overlap - oldest snap wins
        if ((x > 0 && !previousCollisions.testRight()) || (x < 0 && !previousCollisions.testLeft())) {
            obj.position.x += x;
            updated = true;
        }
    } else {
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
    for (auto platform : platforms) {
        for (auto object : objects) {
            auto collision = object->roundedBox.collision(platform->roundedBox);
            if (collision) {
                applyPlatformCollision(*object, *platform, collision);
            }
        }
    }

    for (int i = 0; i < objects.size(); i++) {
        auto a = objects[i];
        for (int j = i + 1; j < objects.size(); j++) {
            auto b = objects[j];
            auto collision = a->roundedBox.collision(b->roundedBox);
            if (collision) {
                applyObjectCollision(*a, *b, collision);
            }
        }
    }
}

void trippin::Engine::applyPlatformCollision(
        trippin::Object &object,
        trippin::Object &platform,
        const Sides &collision) {
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

void trippin::Engine::absorbantCollision(
        trippin::Object &obj,
        trippin::Object &p,
        const trippin::Sides &collision) {
    if ((collision.testLeft() && obj.velocity.x < 0) || (collision.testRight() && obj.velocity.x > 0)) {
        obj.velocity.x = 0;
    }
    if ((collision.testTop() && obj.velocity.y < 0) || (collision.testBottom() && obj.velocity.y > 0)) {
        obj.velocity.y = 0;
    }
    obj.platformCollisions |= collision;
}

void trippin::Engine::reflectiveCollision(
        trippin::Object &object,
        trippin::Object &platform,
        const trippin::Sides &collision) {
    if ((collision.testLeft() && object.velocity.x < 0) || (collision.testRight() && object.velocity.x > 0)) {
        object.velocity.x *= -1;
    }
    if ((collision.testTop() && object.velocity.y < 0) || (collision.testBottom() && object.velocity.y > 0)) {
        object.velocity.y *= -1;
    }
}

void trippin::Engine::applyObjectCollision(
        trippin::Object &left,
        trippin::Object &right,
        const trippin::Sides &collision) {
    left.objectCollisions |= collision;
    right.objectCollisions |= collision.flip();
    left.onObjectCollision(right, collision);
    right.onObjectCollision(left, collision.flip());

    if (testCollision(left, right)) {
        return;
    }

    if (objectCollisionType == ObjectCollisionType::elastic1D) {
        elasticCollision1D(left, right, collision);
    } else if (objectCollisionType == ObjectCollisionType::elastic2D) {
        elasticCollision2D(left, right, collision);
    } else {
        inelasticCollision2D(left, right, collision);
    }
}

void trippin::Engine::elasticCollision1D(
        trippin::Object &obj,
        trippin::Object &p,
        const trippin::Sides &collision) {
    if (rationalHorizontalCollision(obj, p, collision)) {
        auto vels = trippin::elasticCollision1D(obj.velocity.x, p.velocity.x, obj.mass, p.mass);
        obj.velocity.x = vels.first;
        p.velocity.x = vels.second;
        registerCollision(obj, p, obj.velocity, p.velocity);
    }
    if (rationalVerticalCollision(obj, p, collision)) {
        auto vels = trippin::elasticCollision1D(obj.velocity.y, p.velocity.y, obj.mass, p.mass);
        obj.velocity.y = vels.first;
        p.velocity.y = vels.second;
        registerCollision(obj, p, obj.velocity, p.velocity);
    }
}

void trippin::Engine::elasticCollision2D(
        trippin::Object &obj,
        trippin::Object &p,
        const trippin::Sides &collision) {
    if (rationalCollision(obj, p, collision)) {
        auto pair = trippin::elasticCollision2D(obj.velocity, p.velocity, obj.center, p.center, obj.mass, p.mass);
        obj.velocity = pair.first;
        p.velocity = pair.second;
        registerCollision(obj, p, pair.first, pair.second);
    }
}

void trippin::Engine::inelasticCollision2D(
        trippin::Object &obj,
        trippin::Object &p,
        const trippin::Sides &collision) {
    if (rationalCollision(obj, p, collision)) {
        auto pair = trippin::inelasticCollision2D(obj.velocity, p.velocity, obj.center, p.center, obj.mass, p.mass,
                                                  0.9);
        obj.velocity = pair.first;
        p.velocity = pair.second;
        registerCollision(obj, p, pair.first, pair.second);
    }
}

bool trippin::Engine::rationalCollision(
        const trippin::Object &obj,
        const trippin::Object &p,
        const trippin::Sides &collision) {
    return rationalHorizontalCollision(obj, p, collision) || rationalVerticalCollision(obj, p, collision);
}

bool trippin::Engine::rationalHorizontalCollision(
        const trippin::Object &obj,
        const trippin::Object &p,
        const trippin::Sides &collision) {
    return (collision.testLeft() &&
            ((obj.velocity.x < 0 && obj.velocity.x < p.velocity.x) ||
             (p.velocity.x > 0 && p.velocity.x > obj.velocity.x)))
           || (collision.testRight() &&
               ((obj.velocity.x > 0 && obj.velocity.x > p.velocity.x) ||
                (p.velocity.x < 0 && p.velocity.x < obj.velocity.x)));
}

bool trippin::Engine::rationalVerticalCollision(
        const trippin::Object &obj,
        const trippin::Object &p,
        const trippin::Sides &collision) {
    return (collision.testTop() &&
            ((obj.velocity.y < 0 && obj.velocity.y < p.velocity.y) ||
             (p.velocity.y > 0 && p.velocity.y > obj.velocity.y)))
           || (collision.testBottom() &&
               ((obj.velocity.y > 0 && obj.velocity.y > p.velocity.y) ||
                (p.velocity.y < 0 && p.velocity.y < obj.velocity.y)));
}

bool trippin::Engine::testCollision(const trippin::Object &obj, const trippin::Object &p) {
    return obj.id < p.id
           ? prevCollisions.count({obj.id, p.id, obj.velocity, p.velocity})
           : prevCollisions.count({p.id, obj.id, p.velocity, obj.velocity});
}

void trippin::Engine::registerCollision(const trippin::Object &obj, const trippin::Object &p, const Vector<double> &v1,
                                        const Vector<double> &v2) {
    if (obj.id < p.id) {
        nextCollisions.insert({obj.id, p.id, v1, v2});
    } else {
        nextCollisions.insert({p.id, obj.id, v2, v1});
    }
}

void trippin::Engine::setPlatformCollisionType(trippin::PlatformCollisionType t) {
    platformCollisionType = t;
}

void trippin::Engine::setObjectCollisionType(trippin::ObjectCollisionType t) {
    objectCollisionType = t;
}
