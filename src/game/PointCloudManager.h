#ifndef TRIPPIN_POINTCLOUDMANAGER_H
#define TRIPPIN_POINTCLOUDMANAGER_H

#include <array>
#include "sprite/Sprite.h"
#include "engine/Listener.h"
#include "SceneBuilder.h"

namespace trippin {
    class PointCloudManager : public Listener {
    public:
        PointCloudManager(
                Point<int_fast64_t> pointCloudDistanceMin, // engine units
                Point<int_fast64_t> pointCloudDistanceMax, // engine units
                int pointCloudTicks,
                const Sprite &digits,
                SceneBuilder &sceneBuilder,
                Camera &camera,
                int zIndex,
                const Units &units);

        void addPointCloud(Point<int_fast64_t> position, int points, Uint32 ticks);
        void afterTick(Uint32 engineTicks) override;
    private:
        struct PointCloud {
            Point<int_fast64_t> posStart;
            Point<int_fast64_t> posNow;
            Point<int_fast64_t> distance;
            int points{};
            Uint32 ticks;
        };

        const Point<int_fast64_t> pointCloudDistanceMin;
        const Point<int_fast64_t> pointCloudDistanceMax;
        const int pointCloudTicks;
        const Sprite &digits;
        SceneBuilder &sceneBuilder;
        Camera &camera;
        const int zIndex;
        int_fast64_t nextPointCloudPos;
        const Units &units;

        std::array<PointCloud, 12> pointClouds; // circular buffer
    };
}

#endif
