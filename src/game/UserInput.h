#ifndef TRIPPIN_USERINPUT_H
#define TRIPPIN_USERINPUT_H

namespace trippin {
    struct UserInput {
        bool jumpCharge{};
        bool jumpRelease{};
        bool duckStart{};
        bool duckEnd{};

        bool operator!=(const UserInput &ui) const {
            return jumpCharge != ui.jumpCharge
                   || jumpRelease != ui.jumpRelease
                   || duckStart != ui.duckStart
                   || duckEnd != ui.duckEnd;
        }

        operator bool() const {
            return jumpCharge || jumpRelease || duckStart || duckEnd;
        }

        UserInput &operator|=(const UserInput &ui) {
            jumpCharge |= ui.jumpCharge;
            jumpRelease |= ui.jumpRelease;
            duckStart |= ui.duckStart;
            duckEnd |= ui.duckEnd;
            return *this;
        }
    };
}

#endif
