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
                Point<int> pointCloudDistanceMin,
                Point<int> pointCloudDistanceMax,
                int pointCloudTicks,
                const Sprite &digits,
                SceneBuilder &sceneBuilder,
                Camera &camera);

        void addPointCloud(Point<int> position, int points, Uint32 ticks);
        void afterTick(Uint32 engineTicks) override;
    private:
        struct PointCloud {
            Point<int> posStart;
            Point<int> posNow;
            Point<int> distance;
            int points{};
            Uint32 ticks;
        };

        const Point<int> pointCloudDistanceMin;
        const Point<int> pointCloudDistanceMax;
        const int pointCloudTicks;
        const Sprite &digits;
        SceneBuilder &sceneBuilder;
        Camera &camera;

        std::array<PointCloud, 12> pointClouds; // circular buffer
        int nextPointCloudPos;
    };
}

#endif
