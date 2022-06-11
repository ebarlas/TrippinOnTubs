#include "engine/Engine.h"
#include "engine/Timer.h"

void trippin::Engine::add(Object *obj) {
    auto &vec = obj->inactive ? inactive : (obj->platform ? platforms : objects);
    vec.push_back(obj);
}

void trippin::Engine::beforeTick(Uint32 engineTicks) {
    auto fn = [engineTicks](auto obj) { obj->beforeTick(engineTicks); };
    std::for_each(inactive.begin(), inactive.end(), fn);
    std::for_each(platforms.begin(), platforms.end(), fn);
    std::for_each(objects.begin(), objects.end(), fn);
    std::for_each(listeners.begin(), listeners.end(), fn);
}

void trippin::Engine::afterTick(Uint32 engineTicks) {
    auto fn = [engineTicks](auto obj) { obj->afterTick(engineTicks); };
    std::for_each(inactive.begin(), inactive.end(), fn);
    std::for_each(platforms.begin(), platforms.end(), fn);
    std::for_each(objects.begin(), objects.end(), fn);
    std::for_each(listeners.begin(), listeners.end(), fn);
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

void trippin::Engine::tick(Uint32 engineTicks) {
    //SDL_Log("platforms=%lu, objects=%lu", platforms.size(), objects.size());
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
                auto overlap = obj->box.intersect(plat->box);
                if (overlap) {
                    snapTo(*obj, *plat, overlap);
                }
                auto collision = obj->box.collision(plat->box);
                if (collision) {
                    obj->snapCollisions |= collision;
                }
            }
        }
    }
}

void trippin::Engine::snapTo(Object &obj, const Object &p, const Rect<int_fast64_t> &overlap) {
    int_fast64_t x = 0;
    int_fast64_t y = 0;

    // examine horizontal overlap - the overlap rect must align with left-right sides of the two objects
    // this check excludes interior collisions, for example a tall, skinny object that drops vertically in the
    // middle of a wide platform
    if (obj.box.leftAlignedWith(overlap) && p.box.rightAlignedWith(overlap)) {
        x = overlap.w;
    } else if (obj.box.rightAlignedWith(overlap) && p.box.leftAlignedWith(overlap)) {
        x = -overlap.w;
    }

    // examine vertical overlap - the overlap rect must align with top-bottom sides of the two objects
    // this check excludes interior collisions, for example a short, wide object that moves laterally in the
    // middle of a tall platform
    if (obj.box.topAlignedWith(overlap) && p.box.bottomAlignedWith(overlap)) {
        y = overlap.h;
    } else if (obj.box.bottomAlignedWith(overlap) && p.box.topAlignedWith(overlap)) {
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
                if (object->box.hasCollision(platform->box)) {
                    auto collision = object->box.collision(platform->box);
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
                if (a->box.hasCollision(b->box)) {
                    auto collision = a->box.collision(b->box);
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

static void run(trippin::Engine *engine) {
    engine->runEngineLoop();
}

void trippin::Engine::runEngineLoop() {
    SDL_Log("starting engine");
    auto fn = [](int tps) { SDL_Log("timer=engine, tps=%d", tps); };
    Timer timer;
    while (!stopped) {
        {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [this]() { return stopped || ticks.load() < static_cast<int>(ticksPerFrame * renders); });
        }
        auto t = timer.next(fn);
        tick(t);
        ticks = t;
    }
    SDL_Log("stopping engine");
}

void trippin::Engine::start() {
    thread = std::thread(run, this);
}

void trippin::Engine::stop() {
    std::lock_guard<std::mutex> lock{mutex};
    stopped = true;
    cv.notify_one();
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

void trippin::Engine::onRender() {
    std::lock_guard<std::mutex> lg{mutex};
    renders++;
    cv.notify_one();
}

void trippin::Engine::setTicksPerFrame(Fraction<int> tpf) {
    ticksPerFrame = tpf;
}

bool trippin::Engine::sameLane(Object *left, Object *right) {
    if (left->lane == -2 || right->lane == -2) {
        return false;
    }

    if (left->lane == -1 && !right->platform) {
        return false;
    }

    if (right->lane == -1 && !left->platform) {
        return false;
    }

    if (!left->lane || !right->lane) {
        return true;
    }

    return left->lane == right->lane;
}