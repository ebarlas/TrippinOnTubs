#ifndef TRIPPIN_SCENEBUILDER_H
#define TRIPPIN_SCENEBUILDER_H

#include <functional>
#include <vector>
#include <mutex>

namespace trippin {
    // Class for assembling functions that perform scene rendering
    // Engine thread appends functions via reset(), dispatch(fn), then build()
    // Render thread executes all the functions in the latest build via execute()
    //
    // Key properties:
    // - No race conditions
    // - Vectors are reused via swap, never created anew
    // - Narrow, targeted locking that only covers vector swap
    // - No std::function copying beyond initial vector placement
    // - Supports successive calls to execute (without rebuild)
    // - Supports successive builds (without execute)
    //
    // Other considerations:
    // - Execute may be invoked before build, in which case nothing is rendered - this will produce a blank canvas
    class SceneBuilder {
    public:
        void reset();
        void build();
        void dispatch(std::function<void()> drawFn);
        void execute();
    private:
        std::vector<std::function<void()>> workspace; // changes accumulate here via dispatch(...)
        std::vector<std::function<void()>> staging; // vector frozen on build() ready for promotion to scene
        std::vector<std::function<void()>> scene; // vector used for execution promoted from staging
        std::mutex mutex;
        bool updated{};
        void syncScene();
    };
}

#endif
