#ifndef TRIPPIN_NOTIFICATIONMANAGER_H
#define TRIPPIN_NOTIFICATIONMANAGER_H

#include <array>
#include "sprite/Sprite.h"
#include "engine/Listener.h"
#include "SceneBuilder.h"

namespace trippin {
    class NotificationManager : public Listener {
    public:
        NotificationManager(
                const Sprite &text,
                const Sprite &digits,
                double msPerTick,
                int windowWidth,
                int topMargin,
                int rightMargin,
                SceneBuilder &sceneBuilder);
        void add(int value);
        void afterTick(int engineTicks) override;

    private:
        struct Notification {
            int value{};
            int startTicks{};
        };

        static const int DURATION_SLIDE = 1'000;
        static const int DURATION_PAUSE = 2'000;

        const Sprite &text;
        const Sprite &digits;
        const double msPerTick;
        const int windowWidth;
        const int topMargin;
        const int rightMargin;
        SceneBuilder &sceneBuilder;

        std::array<Notification, 3> notifications{};
        unsigned int nextDisplayPos{};

        void render(int engineTicks, Notification &notification);

        static double decelInterpolation(double input);
    };
}

#endif
