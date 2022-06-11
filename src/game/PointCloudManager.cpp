#include "PointCloudManager.h"
#include "ui/DigitLayout.h"
#include "engine/Convert.h"

trippin::PointCloudManager::PointCloudManager(
        Point<int_fast64_t> pointCloudDistanceMin,
        Point<int_fast64_t> pointCloudDistanceMax,
        int pointCloudTicks,
        const trippin::Sprite &digits,
        SceneBuilder &sceneBuilder,
        Camera &camera,
        int zIndex,
        const Units &units) :
        pointCloudDistanceMin(pointCloudDistanceMin),
        pointCloudDistanceMax(pointCloudDistanceMax),
        pointCloudTicks(pointCloudTicks),
        digits(digits),
        sceneBuilder(sceneBuilder),
        camera(camera),
        zIndex(zIndex),
        nextPointCloudPos(0),
        units(units) {

}

void trippin::PointCloudManager::addPointCloud(Point<int_fast64_t> position, int points, Uint32 ticks) {
    auto digitsWidth = units.spriteToEngine(DigitLayout::measureWidth(digits, points));
    auto x = position.x + digitsWidth / 2; // goggin horiz. midpoint
    auto y = position.y;
    auto xRange = pointCloudDistanceMax.x - pointCloudDistanceMin.x;
    auto yRange = pointCloudDistanceMax.y - pointCloudDistanceMin.y;
    double xRand = ((std::rand() * 2.0) / RAND_MAX - 1.0); // [-1.0, 1.0]
    double yRand = (std::rand() * 1.0) / RAND_MAX; // [0.0, 1.0]
    auto xDist = pointCloudDistanceMin.x + static_cast<int_fast64_t>(xRand * xRange);
    auto yDist = pointCloudDistanceMin.y + static_cast<int_fast64_t>(yRand * yRange);
    pointClouds[nextPointCloudPos] = {{x, y}, {x, y}, {xDist, yDist}, points, ticks};
    nextPointCloudPos = (nextPointCloudPos + 1) % pointClouds.size();
}

static float decelInterpolation(float input) {
    return (float) (1.0f - (1.0f - input) * (1.0f - input));
}

void trippin::PointCloudManager::afterTick(Uint32 engineTicks) {
    for (auto &pc: pointClouds) {
        auto elapsed = engineTicks - pc.ticks;
        float di = decelInterpolation(std::min(1.0f, elapsed / (float) pointCloudTicks));
        if (di == 1.0) {
            pc.points = 0; // cancel display
        } else {
            pc.posNow.x = pc.posStart.x + di * pc.distance.x; // x diff may be pos (right) or neg (left)
            pc.posNow.y = pc.posStart.y - di * pc.distance.y; // y diff is always negative (up)
        }
    }

    for (auto &pc: pointClouds) {
        if (pc.points) {
            Point<int_fast64_t> p = pc.posNow;
            auto pts = pc.points;
            sceneBuilder.dispatch([this, p, pts]() {
                DigitLayout::renderDigits(digits, p, pts, &camera, &units);
            }, zIndex);
        }
    }
}
