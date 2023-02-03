#ifndef TRIPPIN_NOTIFICATIONDRAWER_H
#define TRIPPIN_NOTIFICATIONDRAWER_H

#include <array>
#include <functional>
#include "engine/Listener.h"
#include "engine/Point.h"
#include "SceneBuilder.h"

namespace trippin {
    class NotificationDrawer : public Listener {
    public:
        NotificationDrawer(
                double msPerTick,
                int windowWidth,
                int topMargin,
                int rightMargin,
                int padding,
                SceneBuilder &sceneBuilder);
        void add(std::function<void(Point<int>)> renderFn, Point<int> size);
        void afterTick(int engineTicks) override;

    private:
        struct Entry {
            std::function<void(Point<int>)> renderFn;
            Point<int> size;
            int startTicks{};
            int endTicks{};
        };

        static const int DURATION_SLIDE = 1'000;
        static const int DURATION_PAUSE = 2'000;
        static const int DURATION_COLLAPSE = 500;

        const double msPerTick;
        const int windowWidth;
        const int topMargin;
        const int rightMargin;
        const int padding;
        SceneBuilder &sceneBuilder;

        unsigned int firstEntry{};
        unsigned int nextEntry{};
        std::array<Entry, 10> entries{};

        void render(int engineTicks, Entry &entry, int top);
        static double decelInterpolation(double input);
    };
}

#endif
