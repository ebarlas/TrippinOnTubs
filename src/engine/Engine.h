#ifndef TRIPPIN_ENGINE_H
#define TRIPPIN_ENGINE_H

#include <vector>
#include <functional>
#include <thread>
#include <atomic>
#include "SDL.h"
#include "engine/Object.h"
#include "engine/Collisions.h"

namespace trippin {
    // Engine handles the movement and interaction of objects.
    class Engine {
    public:
        // Add an object to the engine. This is a weak reference.
        // The object ought to out-live the engine.
        void add(Object *object);

        // Add non-object engine listener.
        void addListener(Listener *listener);

        // Sets the default type of collision between platform and non-platform objects.
        // This collision type can be overridden by individual objects.
        void setPlatformCollision(std::function<void(Object &, Object &, const Sides &)> collision);

        // Sets the default type of collision between objects.
        // This collision type can be overridden by individual objects.
        void setObjectCollision(std::function<void(Object &, Object &, const Sides &)> collision);

        // Set the target engine tick rate in ticks per second
        void setTickRate(int tickRate);

        // Advance the engine simulation by one tick. In each tick:
        // (1) apply motion to objects and snap to grid
        // (2) detect intersections and snap objects
        // (3) detect and react to collisions
        void tick(int engineTicks);

        void runEngineLoop();

        void start();
        void pause();
        void resume();
        void stop();
        void join();
        int getTicks() const;
        int getMinTps() const;
        int getMaxTps() const;
        int getAvgTps() const;
    private:
        std::vector<Object *> inactive;
        std::vector<Object *> platforms;
        std::vector<Object *> objects;
        std::vector<Listener *> listeners;

        std::function<void(Object &, Object &, const Sides &)> platformCollision = onAbsorbentCollision;
        std::function<void(Object &, Object &, const Sides &)> objectCollision = onInelasticCollisionDefault;

        std::thread thread;
        std::atomic_int tickRate;
        std::atomic_bool paused{};
        std::atomic_bool stopped{};
        std::atomic_int ticks{};
        std::atomic_int minTps{};
        std::atomic_int maxTps{};
        std::atomic_int avgTps{};
        int pauseTicks{};
        int pauseTime{};

        void beforeTick(int engineTicks);
        void afterTick(int engineTicks);
        void promoteActive();
        void removeExpired();
        void applyMotion();
        void snapObjects();
        Object *nextObjectToSnap();
        static bool hasHigherSnapPriorityThan(Object *left, Object *right);
        void snapToPlatform(Object *obj);
        void applyPhysics();
        static void snapTo(Object &obj, const Object &p, const trippin::Rect<int> &overlap);
        void applyPlatformCollision(Object &object, Object &platform, const Sides &sides);
        void applyObjectCollision(Object &left, Object &right, const Sides &sides);

        static bool sameLane(Object *left, Object *right);
    };
}

#endif
