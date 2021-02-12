#ifndef TRIPPIN_ACTIVATION_H
#define TRIPPIN_ACTIVATION_H

#include "Goggin.h"

namespace trippin {
    class Activation {
    private:
        int activationProximity;
        int deactivationProximity;
        const Goggin *goggin;
        Rect<int> universe;
    public:
        void setActivationProximity(int proximity);
        void setDeactivationProximity(int proximity);
        void setGoggin(const Goggin *goggin);
        void setUniverse(const Point<int> &universe);
        bool shouldActivate(const Rect<int> &rect) const;
        bool shouldDeactivate(const Rect<int> &rect) const;
    };
}

#endif
