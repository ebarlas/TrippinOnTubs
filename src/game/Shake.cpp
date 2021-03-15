#include <cstdlib>
#include <cmath>
#include "Shake.h"

void trippin::Shake::init(int pd, int dur) {
    period = pd;
    duration = dur;
    started = false;
    auto count = dur / pd;
    for (int i = 0; i <= count + 1; i++) {
        samples.push_back(std::rand() * 2.0 / RAND_MAX - 1);
    }
}

void trippin::Shake::start(Uint32 ticks) {
    started = true;
    startTime = ticks;
    t = 0;
}

void trippin::Shake::update(Uint32 ticks) {
    t = ticks - startTime;
}

float trippin::Shake::amplitude() {
    if (!started || t >= duration) {
        return 0;
    }

    auto s = static_cast<float>(t) / period;
    auto s0 = (int) std::floor(s);
    auto s1 = s0 + 1;
    auto k = decay();
    return (samples[s0] + (s - s0) * (samples[s1] - samples[s0])) * k;
}

float trippin::Shake::decay() {
    return (duration - static_cast<float>(t)) / duration;
}


