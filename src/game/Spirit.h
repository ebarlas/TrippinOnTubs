#ifndef TRIPPIN_SPIRIT_H
#define TRIPPIN_SPIRIT_H

#include "SDL.h"
#include "engine/Listener.h"

namespace trippin {
    class Spirit : public Listener {
    private:
        double position;
        double velocity;
        double ticksPerSecond;
    public:
        double getPosition() const;
        double getVelocity() const;
        void setTicksPerSecond(double tps);
        void setVelocity(double vel);
        void delay(double seconds);
        void beforeTick(int engineTicks) override;
        void afterTick(int engineTicks) override;
        bool isExpired() override;
    };
}

#endif
