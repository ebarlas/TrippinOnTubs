#include "engine/Engine.h"
#include "engine/Clock.h"

void trippin::Engine::add(Object *obj) {
    auto &vec = obj->inactive ? inactive : (obj->platform ? platforms : objects);
    vec.push_back(obj);
}

void trippin::Engine::beforeTick(int engineTicks) {
    std::for_each(listeners.begin(), listeners.end(), [engineTicks](Listener *obj) { obj->beforeTick(engineTicks); });
}

void trippin::Engine::afterTick(int engineTicks) {
    std::for_each(listeners.begin(), listeners.end(), [engineTicks](Listener *obj) { obj->afterTick(engineTicks); });
}

void trippin::Engine::promoteActive() {
    std::for_each(inactive.begin(), inactive.end(), [this](Object *obj) {
        if (!obj->inactive) {
            auto &vec = obj->platform ? platforms : objects;
            vec.push_back(obj);
        }
    });

    auto it = std::remove_if(inactive.begin(), inactive.end(), [](Object *obj) { return !obj->inactive; });
    inactive.erase(it, inactive.end());
}

void trippin::Engine::removeExpired() {
    auto fn = [](Listener *listener) { return listener->isExpired(); };
    inactive.erase(std::remove_if(inactive.begin(), inactive.end(), fn), inactive.end());
    platforms.erase(std::remove_if(platforms.begin(), platforms.end(), fn), platforms.end());
    objects.erase(std::remove_if(objects.begin(), objects.end(), fn), objects.end());
    listeners.erase(std::remove_if(listeners.begin(), listeners.end(), fn), listeners.end());
}

void trippin::Engine::tick(int engineTicks) {
    // SDL_Log("inactive=%d, platforms=%d, objects=%d", inactive.size(), platforms.size(), objects.size());
    beforeTick(engineTicks);
    promoteActive();
    removeExpired();
    applyMotion();
    snapObjects();
    applyPhysics();
    afterTick(engineTicks);
}

void trippin::Engine::applyMotion() {
    std::for_each(objects.begin(), objects.end(), [](Object *obj) { obj->applyMotion(); });
}

void trippin::Engine::snapObjects() {
    // first, snap objects to each platform
    for (auto plat : platforms) {
        snapToPlatform(plat);
    }

    // next, snap objects to each object in priority order
    for (auto *it = nextObjectToSnap(); it != nullptr; it = nextObjectToSnap()) {
        snapToPlatform(it);
    }
}

trippin::Object *trippin::Engine::nextObjectToSnap() {
    Object *max = nullptr;
    for (auto obj : objects) {
        if (!obj->snappedToMe && (max == nullptr || hasHigherSnapPriorityThan(obj, max))) {
            max = obj;
        }
    }
    if (max != nullptr) {
        max->snappedToMe = true;
    }
    return max;
}

bool trippin::Engine::hasHigherSnapPriorityThan(Object *left, Object *right) {
    // object with most collision sides is highest priority
    auto lcnt = left->snapCollisions.count();
    auto rcnt = right->snapCollisions.count();
    if (lcnt != rcnt)
        return lcnt > rcnt;

    // object with highest y position (lower on screen) is next highest priority
    return left->position.y > right->position.y;
}

void trippin::Engine::snapToPlatform(Object *plat) {
    for (auto obj : objects) {
        if (!obj->snappedToMe) {
            if (sameLane(obj, plat)) {
                auto overlap = obj->roundedBox.intersect(plat->roundedBox);
                if (overlap) {
                    snapTo(*obj, *plat, overlap);
                }
                auto collision = obj->roundedBox.collision(plat->roundedBox);
                if (collision) {
                    obj->snapCollisions |= collision;
                }
            }
        }
    }
}

void trippin::Engine::snapTo(Object &obj, const Object &p, const Rect<int> &overlap) {
    auto x = 0.0;
    auto y = 0.0;

    // examine horizontal overlap - the overlap rect must align with left-right sides of the two objects
    // this check excludes interior collisions, for example a tall, skinny object that drops vertically in the
    // middle of a wide platform
    if (obj.roundedBox.leftAlignedWith(overlap) && p.roundedBox.rightAlignedWith(overlap)) {
        x = overlap.w;
    } else if (obj.roundedBox.rightAlignedWith(overlap) && p.roundedBox.leftAlignedWith(overlap)) {
        x = -overlap.w;
    }

    // examine vertical overlap - the overlap rect must align with top-bottom sides of the two objects
    // this check excludes interior collisions, for example a short, wide object that moves laterally in the
    // middle of a tall platform
    if (obj.roundedBox.topAlignedWith(overlap) && p.roundedBox.bottomAlignedWith(overlap)) {
        y = overlap.h;
    } else if (obj.roundedBox.bottomAlignedWith(overlap) && p.roundedBox.topAlignedWith(overlap)) {
        y = -overlap.h;
    }

    // use smallest displacement
    if (x != 0 && (y == 0 || (std::abs(x) < std::abs(y)))) {
        // dont snap back over previous overlap - oldest snap wins
        if ((x > 0 && !obj.snapCollisions.testRight()) || (x < 0 && !obj.snapCollisions.testLeft())) {
            obj.position.x += x;
            obj.syncPositions();
        }
    } else if (y != 0) {
        // same as above
        if ((y > 0 && !obj.snapCollisions.testBottom()) || (y < 0 && !obj.snapCollisions.testTop())) {
            obj.position.y += y;
            obj.syncPositions();
        }
    }
}

void trippin::Engine::applyPhysics() {
    for (auto platform : platforms) {
        for (auto object : objects) {
            if (sameLane(object, platform)) {
                if (object->roundedBox.hasCollision(platform->roundedBox)) {
                    auto collision = object->roundedBox.collision(platform->roundedBox);
                    applyPlatformCollision(*object, *platform, collision);
                }
            }
        }
    }

    for (int i = 0; i < objects.size(); i++) {
        auto a = objects[i];
        for (int j = i + 1; j < objects.size(); j++) {
            auto b = objects[j];
            if (sameLane(a, b)) {
                if (a->roundedBox.hasCollision(b->roundedBox)) {
                    auto collision = a->roundedBox.collision(b->roundedBox);
                    applyObjectCollision(*a, *b, collision);
                }
            }
        }
    }
}

void trippin::Engine::applyPlatformCollision(Object &object, Object &platform, const Sides &sides) {
    auto collision = object.platformCollision.isPresent()
                     ? object.platformCollision.get()
                     : platformCollision;
    object.onPlatformCollision(platform, sides);
    collision(object, platform, sides);
}

void trippin::Engine::applyObjectCollision(Object &left, Object &right, const Sides &sides) {
    // this is a strange policy, but it supports object-level object-collision overrides
    auto collision = left.objectCollision.isPresent() && right.objectCollision.isPresent()
            ? left.objectCollision.get()
            : objectCollision;
    left.objectCollisions |= sides;
    right.objectCollisions |= sides.flip();
    left.onObjectCollision(right, sides);
    right.onObjectCollision(left, sides.flip());
    collision(left, right, sides);
}

void trippin::Engine::setTickRate(int tps) {
    tickRate = tps;
}

static void run(trippin::Engine *engine) {
    engine->runEngineLoop();
}

void trippin::Engine::runEngineLoop() {
    Clock clock{tickRate};
    while (!stopped) {
        if (paused) {
            if (!pauseTicks) {
                pauseTicks = clock.getTicks();
            }
        } else {
            auto clockTicks = clock.getTicks();
            if (pauseTicks) {
                pauseTime += (clockTicks - pauseTicks);
                pauseTicks = 0;
            }
            tick(clockTicks - pauseTime);
            ticks++;
        }
        auto &clockStats = clock.getStats();
        minTps = clockStats.getMin();
        maxTps = clockStats.getMax();
        avgTps = clockStats.getAvg();
        clock.next();
        if (tickRate != clock.getTickRate()) {
            clock.updateTickRate(tickRate);
        }
    }
}

void trippin::Engine::start() {
    thread = std::thread(run, this);
}

void trippin::Engine::pause() {
    paused = true;
}

void trippin::Engine::resume() {
    paused = false;
}

void trippin::Engine::stop() {
    stopped = true;
}

void trippin::Engine::join() {
    thread.join();
}

void trippin::Engine::setPlatformCollision(std::function<void(Object&, Object&, const Sides&)> collision) {
    platformCollision = std::move(collision);
}

void trippin::Engine::setObjectCollision(std::function<void(Object&, Object&, const Sides&)> collision) {
    objectCollision = std::move(collision);
}

void trippin::Engine::addListener(trippin::Listener *listener) {
    listeners.push_back(listener);
}

int trippin::Engine::getTicks() const {
    return ticks;
}

int trippin::Engine::getMinTps() const {
    return minTps;
}

int trippin::Engine::getMaxTps() const {
    return maxTps;
}

int trippin::Engine::getAvgTps() const {
    return avgTps;
}

bool trippin::Engine::sameLane(Object *left, Object *right) {
    if (left->lane == -3 && right->platform) { // lane -3 denotes no-platform collisions
        return false;
    }
    if (right->lane == -3 && left->platform) {
        return false;
    }

    if (left->lane == -2 || right->lane == -2) { // lane -2 denotes no collisions whatsoever
        return false;
    }

    if (left->lane == -1 && !right->platform) { // lane -1 denotes only platform collisions
        return false;
    }
    if (right->lane == -1 && !left->platform) {
        return false;
    }

    if (!left->lane || !right->lane) { // no lane denotes collisions with all lanes
        return true;
    }

    return left->lane == right->lane;
}