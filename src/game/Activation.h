#ifndef TRIPPIN_ACTIVATION_H
#define TRIPPIN_ACTIVATION_H

#include "Goggin.h"

namespace trippin {
    class Activation {
    private:
        int_fast64_t activationProximity;
        int_fast64_t deactivationProximity;
        const Goggin *goggin;
        Rect<int_fast64_t> universe;
    public:
        void setActivationProximity(int_fast64_t proximity);  // engine scale
        void setDeactivationProximity(int_fast64_t proximity);  // engine scale
        void setGoggin(const Goggin *goggin);
        void setUniverse(const Point<int_fast64_t> &universe); // engine scale
        bool shouldActivate(const Rect<int_fast64_t> &rect) const;
        bool shouldDeactivate(const Rect<int_fast64_t> &rect) const;
    };
}

#endif
