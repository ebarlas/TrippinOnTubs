#ifndef TRIPPIN_SCENEBUILDER_H
#define TRIPPIN_SCENEBUILDER_H

#include <functional>
#include <vector>
#include <mutex>
#include <memory>

namespace trippin {
    class SceneBuilder {
    public:
        SceneBuilder();
        void reset();
        void build();
        void dispatch(std::function<void()> drawFn);
        void execute();
    private:
        std::unique_ptr<std::vector<std::function<void()>>> workspace; // changes accumulate here via dispatch(...)
        std::unique_ptr<std::vector<std::function<void()>>> staging; // vector frozen on build() ready for promotion to scene
        std::unique_ptr<std::vector<std::function<void()>>> scene; // vector used for execution promoted from staging
        std::mutex mutex;
        bool updated;
        void syncScene();
    };
}

#endif
