#ifndef TRIPPIN_SCENEBUILDER_H
#define TRIPPIN_SCENEBUILDER_H

#include <functional>
#include <vector>
#include <mutex>

namespace trippin {
    class SceneBuilder {
    public:
        struct DrawFn {
            int zIndex = 0;
            std::function<void()> fn;
        };

        void reset();
        void build();
        void dispatch(std::function<void()> drawFn, int zIndex = 1'000);
        std::vector<DrawFn> take();
    private:
        std::mutex mutex;
        std::vector<DrawFn> drawFns;
        std::vector<DrawFn> scene;
    };
}

#endif
