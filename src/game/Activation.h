#ifndef TRIPPIN_ACTIVATION_H
#define TRIPPIN_ACTIVATION_H

#include "Goggin.h"

namespace trippin {
    class Activation {
    private:
        int proximity;
        const Goggin *goggin;
    public:
        void setProximity(int proximity);
        void setGoggin(const Goggin *goggin);
        bool shouldActivate(int position) const;
    };
}

#endif
