#include <cmath>
#include "Shake.h"
#include "Random.h"

void trippin::Shake::init(int pd, int dur) {
    period = pd;
    duration = dur;
    started = false;
    Random<> rand;
    auto count = dur / pd;
    for (int i = 0; i <= count + 1; i++) {
        samples.push_back(static_cast<float>(rand.next() * 2.0 / rand.max() - 1));
    }
}

void trippin::Shake::start(int ticks) {
    started = true;
    startTime = ticks;
    t = 0;
}

void trippin::Shake::update(int ticks) {
    t = ticks - startTime;
}

float trippin::Shake::amplitude() {
    if (!started || t >= duration) {
        return 0;
    }

    // time offset in period-units
    auto s = static_cast<float>(t) / static_cast<float>(period);

    // lower sample
    auto s0 = static_cast<unsigned int>(std::floor(s));

    // upper sample
    auto s1 = s0 + 1;

    // dampening factor proportional to progress
    auto k = (static_cast<float>(duration) - static_cast<float>(t)) / static_cast<float>(duration);

    // interpolate from lower sample to upper, then dampen
    return (samples[s0] + (s - static_cast<float>(s0)) * (samples[s1] - samples[s0])) * k;
}


