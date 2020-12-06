#ifndef TRIPPIN_SPIRIT_H
#define TRIPPIN_SPIRIT_H

#include "SDL.h"
#include "engine/Listener.h"

namespace trippin {
    class Spirit : public Listener {
    private:
        double position;
        double velocity;
    public:
        double getPosition() const;
        void setPosition(double pos);
        void setVelocity(double vel);
        void updatePosition(double delta);
        void beforeTick(Uint32 engineTicks) override;
        void afterTick(Uint32 engineTicks) override;
    };
}

#endif
