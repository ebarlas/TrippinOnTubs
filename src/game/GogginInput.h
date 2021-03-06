#ifndef TRIPPIN_GOGGININPUT_H
#define TRIPPIN_GOGGININPUT_H

namespace trippin {
    struct GogginInput {
        bool jumpCharge{};
        bool jumpRelease{};
        bool duckStart{};
        bool duckEnd{};

        bool operator!=(const GogginInput &ui) const {
            return jumpCharge != ui.jumpCharge
                   || jumpRelease != ui.jumpRelease
                   || duckStart != ui.duckStart
                   || duckEnd != ui.duckEnd;
        }

        operator bool() const {
            return jumpCharge || jumpRelease || duckStart || duckEnd;
        }

        GogginInput &operator|=(const GogginInput &ui) {
            jumpCharge |= ui.jumpCharge;
            jumpRelease |= ui.jumpRelease;
            duckStart |= ui.duckStart;
            duckEnd |= ui.duckEnd;
            return *this;
        }
    };
}

#endif
