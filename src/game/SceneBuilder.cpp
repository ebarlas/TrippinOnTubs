#include "SceneBuilder.h"

void trippin::SceneBuilder::reset() {
    drawFns.clear();
}

void trippin::SceneBuilder::build() {
    std::lock_guard<std::mutex> lock(mutex);
    scene = std::move(drawFns);
}

void trippin::SceneBuilder::dispatch(std::function<void()> drawFn) {
    drawFns.push_back(std::move(drawFn));
}

void trippin::SceneBuilder::execute() {
    static std::vector<std::function<void()>> fns;
    {
        std::lock_guard<std::mutex> lock(mutex);
        fns = scene;
    }
    for (auto &fn: fns) {
        fn();
    }
}