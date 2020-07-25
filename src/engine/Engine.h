#ifndef TRIPPIN_ENGINE_H
#define TRIPPIN_ENGINE_H

#include <vector>
#include <unordered_set>
#include "SDL.h"
#include "engine/Object.h"
#include "engine/CollisionType.h"
#include "engine/Grid.h"

namespace trippin {
    // Engine handles the movement and interaction of objects.
    class Engine {
    public:
        // Add an object to the engine. The is a weak reference.
        // The object ought to out-live the engine.
        void add(Object *object);

        // Set the size of the spacial partitioning grid.
        // gridSize is the cols (x) and rows (y) of the grid.
        // cellSize is the uniform width (x) and height (y) of each grid cell.
        void setGridSize(Point<int> gridSize, Point<int> cellSize);

        // Sets the default type of collision between platform and non-platform objects.
        // This collision type can be overridden by individual objects.
        void setPlatformCollisionType(PlatformCollisionType t);

        // Sets the default type of collision between objects.
        // This collision type can be overridden by individual objects.
        void setObjectCollisionType(ObjectCollisionType t);

        // Restitution coefficient for inelastic collisions.
        void setRestitutionCoefficient(double r);

        // Set the target engine tick period. Ticks per second are 1000 / tick-period.
        // A tick period of 10 results in 100 tps.
        void setTickPeriod(int tp);

        // Advance the engine simulation by one tick. In each tick:
        // (1) apply motion to objects and snap to grid
        // (2) detect intersections and snap objects
        // (3) detect and react to collisions
        void tick();

        void runEngineLoop();

        void start();

        void pause();

        void stop();
    private:
        Grid grid{};
        std::vector<Object *> platforms{};
        std::vector<Object *> objects{};
        PlatformCollisionType platformCollisionType = PlatformCollisionType::absorbant;
        ObjectCollisionType objectCollisionType = ObjectCollisionType::inelastic;

        double restitutionCoefficient = 0.9;

        SDL_Thread *thread;
        int tickPeriod = 10;
//        int ticksPerSecond = 180;
//        int threadPeriod = 4;
        bool paused = false;
        bool stopped = false;

        void beforeTick();
        void afterTick();
        void applyMotion();
        void snapObjects();
        void snapObjects(Partition &partition);
        void applyPhysics();
        void snapTo(Object &obj, const Object &p, const trippin::Rect<int> &overlap, const Sides &previousContacts);

        void applyPlatformCollision(Object &object, Object &platform, const Sides &collision);
        void applyObjectCollision(Object &left, Object &right, const Sides &collision);
        void absorbantCollision(Object &object, Object &platform, const Sides &collision);
        void reflectiveCollision(Object &object, Object &platform, const Sides &collision);
        void elasticCollision1D(Object &object, Object &platform, const Sides &collision);
        void elasticCollision2D(Object &object, Object &platform, const Sides &collision);
        void inelasticCollision2D(Object &object, Object &platform, const Sides &collision);

        static bool rationalCollision(const trippin::Object &obj, const Object &p, const Sides &collision);
        static bool rationalHorizontalCollision(const trippin::Object &obj, const Object &p, const Sides &collision);
        static bool rationalVerticalCollision(const trippin::Object &obj, const Object &p, const Sides &collision);
    };
}

#endif
