#ifndef TRIPPIN_SCENEBUILDER_H
#define TRIPPIN_SCENEBUILDER_H

#include <functional>
#include <vector>
#include <mutex>

namespace trippin {
    class SceneBuilder {
    public:
        void reset();
        void build();
        void dispatch(std::function<void()> drawFn);
        void execute();
    private:
        std::mutex mutex;
        std::vector<std::function<void()>> drawFns; // changes accumulate here via dispatch(...)
        std::vector<std::function<void()>> scene; // scene is frozen on build()
    };
}

#endif
