#include "SceneBuilder.h"

void trippin::SceneBuilder::reset() {
    drawFns.clear();
}

void trippin::SceneBuilder::build() {
    std::lock_guard<std::mutex> lock(mutex);
    scene = std::move(drawFns);
}

void trippin::SceneBuilder::dispatch(std::function<void()> drawFn, int zIndex) {
    drawFns.push_back({zIndex, std::move(drawFn)});
}

std::vector<trippin::SceneBuilder::DrawFn> trippin::SceneBuilder::take() {
    std::lock_guard<std::mutex> lock(mutex);
    return scene;
}
