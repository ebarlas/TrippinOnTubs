#include "PointCloudManager.h"
#include "ui/DigitLayout.h"

trippin::PointCloudManager::PointCloudManager(
        Point<int> pointCloudDistanceMin,
        Point<int> pointCloudDistanceMax,
        int pointCloudTicks,
        const trippin::Sprite &digits,
        SceneBuilder &sceneBuilder,
        Camera &camera) :
        pointCloudDistanceMin(pointCloudDistanceMin),
        pointCloudDistanceMax(pointCloudDistanceMax),
        pointCloudTicks(pointCloudTicks),
        digits(digits),
        sceneBuilder(sceneBuilder),
        camera(camera),
        nextPointCloudPos(0) {

}

void trippin::PointCloudManager::addPointCloud(Point<int> position, int points, int ticks) {
    auto midpoint = DigitLayout::measureWidth(digits, points) / 2; // goggin horiz. midpoint
    int x = position.x + digits.getScale().getDeviceEngineFactor() * midpoint;
    int y = position.y;
    int xRange = pointCloudDistanceMax.x - pointCloudDistanceMin.x;
    int yRange = pointCloudDistanceMax.y - pointCloudDistanceMin.y;
    double xRand = ((random.next() * 2.0) / random.max() - 1.0); // [-1.0, 1.0]
    double yRand = (random.next() * 1.0) / random.max(); // [0.0, 1.0]
    auto xDist = pointCloudDistanceMin.x + static_cast<int>(xRand * xRange);
    auto yDist = pointCloudDistanceMin.y + static_cast<int>(yRand * yRange);
    pointClouds[nextPointCloudPos] = {{x, y}, {x, y}, {xDist, yDist}, points, ticks};
    nextPointCloudPos = (nextPointCloudPos + 1) % pointClouds.size();
}

static float decelInterpolation(float input) {
    return (1.0f - (1.0f - input) * (1.0f - input));
}

void trippin::PointCloudManager::afterTick(int engineTicks) {
    for (auto &pc: pointClouds) {
        auto elapsed = engineTicks - pc.ticks;
        auto progress = static_cast<float>(elapsed) / static_cast<float>(pointCloudTicks);
        auto di = decelInterpolation(std::min(1.0f, progress));
        if (di == 1.0f) {
            pc.points = 0; // cancel display
        } else {
            // x diff may be pos (right) or neg (left), y diff is always negative (up)
            pc.posNow.x = pc.posStart.x + static_cast<int>(di * static_cast<float>(pc.distance.x));
            pc.posNow.y = pc.posStart.y - static_cast<int>(di * static_cast<float>(pc.distance.y));
        }
    }

    for (auto &pc: pointClouds) {
        if (pc.points) {
            Point<int> p = pc.posNow;
            auto pts = pc.points;
            sceneBuilder.dispatch([this, p, pts]() {
                DigitLayout::renderDigits(digits, p, pts, &camera);
            });
        }
    }
}
