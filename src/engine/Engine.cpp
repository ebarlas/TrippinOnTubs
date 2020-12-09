#include "SDL.h"
#include "engine/Engine.h"
#include "engine/SnapQueue.h"
#include "engine/Clock.h"

void trippin::Engine::add(Object *obj) {
    auto &vec = obj->platform ? platforms : objects;
    vec.push_back(obj);
}

void trippin::Engine::remove(Object *obj) {
    auto &vec = obj->platform ? platforms : objects;
    vec.erase(std::find(vec.begin(), vec.end(), obj));
}

void trippin::Engine::forEachObject(std::function<void(Object * )> fn) {
    std::for_each(platforms.begin(), platforms.end(), fn);
    std::for_each(objects.begin(), objects.end(), fn);
}

void trippin::Engine::forEachListener(std::function<void(Listener * )> fn) {
    std::for_each(listeners.begin(), listeners.end(), fn);
}

void trippin::Engine::beforeTick(Uint32 engineTicks) {
    forEachObject([engineTicks](Object *obj) { obj->beforeTick(engineTicks); });
    forEachListener([engineTicks](Listener *listener) { listener->beforeTick(engineTicks); });
}

void trippin::Engine::afterTick(Uint32 engineTicks) {
    forEachObject([engineTicks](Object *obj) { obj->afterTick(engineTicks); });
    forEachListener([engineTicks](Listener *listener) { listener->afterTick(engineTicks); });
}

void trippin::Engine::removeExpired() {
    auto fn = [](Listener *obj) { return obj->isExpired(); };
    platforms.erase(std::remove_if(platforms.begin(), platforms.end(), fn), platforms.end());
    objects.erase(std::remove_if(objects.begin(), objects.end(), fn), objects.end());
    listeners.erase(std::remove_if(listeners.begin(), listeners.end(), fn), listeners.end());
}

void trippin::Engine::tick(Uint32 engineTicks) {
    beforeTick(engineTicks);
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
    forEachObject([](Object *obj) { obj->snapCollisions.clear(); });
    forEachObject([this](Object *obj) { snapQueue.push(obj); });

    // Traverse objects in priority order where priority is determined by platform contacts.
    // Each object acts like a platform once. All non-platform, overlapping neighbor objects are snapped.
    while (!snapQueue.empty()) {
        auto plat = snapQueue.pop();
        for (auto kin : objects) {
            if (plat != kin && !kin->queueVisited) {
                auto overlap = kin->roundedBox.intersect(plat->roundedBox);
                if (overlap) {
                    snapTo(*kin, *plat, overlap, kin->snapCollisions);
                }
                auto collision = kin->roundedBox.collision(plat->roundedBox);
                if (collision) {
                    auto c = kin->snapCollisions |= collision;
                    snapQueue.push(kin, c);
                }
            }
        }
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
        obj.syncPositions();
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

void trippin::Engine::applyPlatformCollision(Object &object, Object &platform, const Sides &sides) {
    auto collision = object.platformCollision.isPresent()
                ? object.platformCollision.get()
                : platformCollision;
    object.onPlatformCollision(platform, sides);
    collision->onCollision(object, platform, sides);
}

void trippin::Engine::applyObjectCollision(Object &left, Object &right, const Sides &sides) {
    left.objectCollisions |= sides;
    right.objectCollisions |= sides.flip();
    left.onObjectCollision(right, sides);
    right.onObjectCollision(left, sides.flip());
    objectCollision->onCollision(left, right, sides);
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
    Clock clock{static_cast<Uint32>(tickPeriod)};
    while (!stopped) {
        if (!paused) {
            tick(clock.getTicks());
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

void trippin::Engine::setPlatformCollision(trippin::Collision *collision) {
    platformCollision = collision;
}

void trippin::Engine::setObjectCollision(trippin::Collision *collision) {
    objectCollision = collision;
}

void trippin::Engine::addListener(trippin::Listener *listener) {
    listeners.push_back(listener);
}
