#ifndef TRIPPIN_SPIRIT_H
#define TRIPPIN_SPIRIT_H

#include "SDL.h"
#include "engine/Listener.h"

namespace trippin {
    class Spirit : public Listener {
    private:
        int_fast64_t position;
        int_fast64_t velocity;
        int ticksPerSecond;
    public:
        int_fast64_t getPosition() const;
        int_fast64_t getVelocity() const;
        void setTicksPerSecond(int tps);
        void setVelocity(int_fast64_t vel);
        void delay(int seconds);
        void beforeTick(Uint32 engineTicks) override;
        void afterTick(Uint32 engineTicks) override;
    };
}

#endif
