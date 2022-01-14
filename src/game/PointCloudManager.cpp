#include "PointCloudManager.h"
#include "ui/DigitLayout.h"
#include "engine/Convert.h"

trippin::PointCloudManager::PointCloudManager(
        Point<int> pointCloudDistanceMin,
        Point<int> pointCloudDistanceMax,
        int pointCloudTicks,
        const trippin::Sprite &digits,
        SceneBuilder &sceneBuilder,
        Camera &camera,
        int zIndex) :
        pointCloudDistanceMin(pointCloudDistanceMin),
        pointCloudDistanceMax(pointCloudDistanceMax),
        pointCloudTicks(pointCloudTicks),
        digits(digits),
        sceneBuilder(sceneBuilder),
        camera(camera),
        zIndex(zIndex),
        nextPointCloudPos(0) {

}

void trippin::PointCloudManager::addPointCloud(Point<int> position, int points, Uint32 ticks) {
    int x = position.x + DigitLayout::measureWidth(digits, points) / 2; // goggin horiz. midpoint
    int y = position.y;
    int xRange = pointCloudDistanceMax.x - pointCloudDistanceMin.x;
    int yRange = pointCloudDistanceMax.y - pointCloudDistanceMin.y;
    double xRand = ((std::rand() * 2.0) / RAND_MAX - 1.0); // [-1.0, 1.0]
    double yRand = (std::rand() * 1.0) / RAND_MAX; // [0.0, 1.0]
    auto xDist = pointCloudDistanceMin.x + toInt(xRand * xRange);
    auto yDist = pointCloudDistanceMin.y + toInt(yRand * yRange);
    pointClouds[nextPointCloudPos] = {{x, y}, {x, y}, {xDist, yDist}, points, ticks};
    nextPointCloudPos = (nextPointCloudPos + 1) % pointClouds.size();
}

static float decelInterpolation(float input) {
    return (float) (1.0f - (1.0f - input) * (1.0f - input));;
}

void trippin::PointCloudManager::afterTick(Uint32 engineTicks) {
    for (auto &pc: pointClouds) {
        auto elapsed = engineTicks - pc.ticks;
        float di = decelInterpolation(std::min(1.0f, elapsed / (float) pointCloudTicks));
        if (di == 1.0) {
            pc.points = 0; // cancel display
        } else {
            pc.posNow.x = pc.posStart.x + toInt(di * pc.distance.x); // x diff may be pos (right) or neg (left)
            pc.posNow.y = pc.posStart.y - toInt(di * pc.distance.y); // y diff is always negative (up)
        }
    }

    for (auto &pc: pointClouds) {
        if (pc.points) {
            Point<int> p = pc.posNow;
            auto pts = pc.points;
            sceneBuilder.dispatch([this, p, pts]() {
                DigitLayout::renderDigits(digits, p, pts, &camera);
            }, zIndex);
        }
    }
}
