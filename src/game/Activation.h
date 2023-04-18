#ifndef TRIPPIN_ACTIVATION_H
#define TRIPPIN_ACTIVATION_H

#include "Goggin.h"

namespace trippin {
    class Activation {
    private:
        const int activationProximity;
        const int deactivationProximity;
        const Goggin &goggin;
        const Rect<int> universe;
    public:
        Activation(
                int activationProximity,
                int deactivationProximity,
                const Goggin &goggin,
                Rect<int> universe);
        bool shouldActivate(const Rect<int> &rect) const;
        bool shouldDeactivate(const Rect<int> &rect) const;
    };
}

#endif
