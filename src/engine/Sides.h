#ifndef TRIPPIN_SIDES_H
#define TRIPPIN_SIDES_H

#include <bitset>

namespace trippin {
    class Sides {
    private:
        enum Side {
            left, top, bottom, right
        };
        std::bitset<4> sides;
    public:
        Sides &operator|=(const Sides &c);
        bool operator==(const Sides &c) const;
        operator bool() const;
        operator int() const;
        int count() const;
        void set(Side side, bool on);
        bool test(Side side) const;
        bool testLeft() const;
        bool testTop() const;
        bool testRight() const;
        bool testBottom() const;
        void setLeft(bool on);
        void setTop(bool on);
        void setRight(bool on);
        void setBottom(bool on);
        Sides flip() const;
    };
}

#endif