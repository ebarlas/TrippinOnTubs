#include "SceneBuilder.h"

trippin::SceneBuilder::SceneBuilder() :
        workspace(std::make_unique<std::vector<std::function<void()>>>()),
        staging(std::make_unique<std::vector<std::function<void()>>>()),
        scene(std::make_unique<std::vector<std::function<void()>>>()),
        updated(false) {
}

void trippin::SceneBuilder::reset() {
    workspace->clear();
}

void trippin::SceneBuilder::build() {
    std::lock_guard<std::mutex> lock(mutex);
    workspace.swap(staging);
    updated = true;
}

void trippin::SceneBuilder::dispatch(std::function<void()> drawFn) {
    workspace->push_back(std::move(drawFn));
}

void trippin::SceneBuilder::execute() {
    syncScene();
    for (auto &fn: *scene) {
        fn();
    }
}

void trippin::SceneBuilder::syncScene() {
    std::lock_guard<std::mutex> lock(mutex);
    if (updated) {
        staging.swap(scene);
        updated = false;
    }
}