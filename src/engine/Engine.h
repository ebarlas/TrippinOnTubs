#ifndef TRIPPIN_ENGINE_H
#define TRIPPIN_ENGINE_H

#include <vector>
#include <functional>
#include "SDL.h"
#include "engine/Object.h"
#include "engine/Collision.h"
#include "engine/AbsorbentCollision.h"
#include "engine/InelasticCollision.h"
#include "engine/Clock.h"

namespace trippin {
    // Engine handles the movement and interaction of objects.
    class Engine {
    public:
        // Add an object to the engine. The is a weak reference.
        // The object ought to out-live the engine.
        void add(Object *object);

        // Add non-object engine listener.
        void addListener(Listener *listener);

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
        void resume();
        void stop();
        void join();
    private:
        AbsorbentCollision defaultPlatformCollision;
        InelasticCollision defaultObjectCollision;

        std::vector<Object *> inactive;
        std::vector<Object *> platforms;
        std::vector<Object *> objects;
        std::vector<Listener *> listeners;
        Collision *platformCollision = &defaultPlatformCollision;
        Collision *objectCollision = &defaultObjectCollision;

        SDL_Thread *thread;
        int tickPeriod;
        SDL_atomic_t paused{};
        SDL_atomic_t stopped{};
        Uint32 pauseTicks{};
        Uint32 pauseTime{};

        void beforeTick(Uint32 engineTicks);
        void afterTick(Uint32 engineTicks);
        void promoteActive();
        void removeExpired();
        void applyMotion();
        void snapObjects();
        Object* nextObjectToSnap();
        static bool hasHigherSnapPriorityThan(Object *left, Object *right);
        void snapToPlatform(Object *obj);
        void applyPhysics();
        static void snapTo(Object &obj, const Object &p, const trippin::Rect<int> &overlap);
        void applyPlatformCollision(Object &object, Object &platform, const Sides &sides);
        void applyObjectCollision(Object &left, Object &right, const Sides &sides);

        inline static bool sameLane(Object* left, Object* right);
    };
}

#endif
