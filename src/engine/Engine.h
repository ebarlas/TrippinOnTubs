#ifndef TRIPPIN_ENGINE_H
#define TRIPPIN_ENGINE_H

#include <vector>
#include <functional>
#include "SDL.h"
#include "engine/Object.h"
#include "engine/Collision.h"
#include "engine/AbsorbentCollision.h"
#include "engine/Clock.h"
#include "SnapQueue.h"

namespace trippin {
    // Engine handles the movement and interaction of objects.
    class Engine {
    public:
        // Add an object to the engine. The is a weak reference.
        // The object ought to out-live the engine.
        void add(Object *object);

        // Remove an object from the engine.
        void remove(Object *object);

        // Sets the default type of collision between platform and non-platform objects.
        // This collision type can be overridden by individual objects.
        void setPlatformCollision(Collision *collision);

        // Sets the default type of collision between objects.
        // This collision type can be overridden by individual objects.
        void setObjectCollision(Collision *collision);

        // Set the target engine tick period. Ticks per second are 1000 / tick-period.
        // A tick period of 10 results in 100 tps.
        void setTickPeriod(int tp);

        // Advance the engine simulation by one tick. In each tick:
        // (1) apply motion to objects and snap to grid
        // (2) detect intersections and snap objects
        // (3) detect and react to collisions
        void tick(Uint32 engineTicks);

        void runEngineLoop();

        void start();

        void pause();

        void stop();
    private:
        std::vector<Object *> platforms{};
        std::vector<Object *> objects{};
        Collision *platformCollision{};
        Collision *objectCollision{};

        SDL_Thread *thread;
        int tickPeriod;
        bool paused = false;
        bool stopped = false;

        // This data structure is used to prioritize objects during object snapping
        SnapQueue snapQueue{};

        void beforeTick(Uint32 engineTicks);
        void afterTick(Uint32 engineTicks);
        void removeExpired();
        void applyMotion();
        void snapObjects();
        void applyPhysics();
        void forEachObject(std::function<void(Object *)> fn);
        void snapTo(Object &obj, const Object &p, const trippin::Rect<int> &overlap, const Sides &previousContacts);

        void applyPlatformCollision(Object &object, Object &platform, const Sides &sides);
        void applyObjectCollision(Object &left, Object &right, const Sides &sides);
    };
}

#endif
