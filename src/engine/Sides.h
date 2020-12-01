#ifndef TRIPPIN_SIDES_H
#define TRIPPIN_SIDES_H

#include <ostream>
#include <bitset>

namespace trippin {
    class Sides {
    private:
        enum Side {
            left, top, bottom, right
        };
        std::bitset<4> sides;
        void set(Side side, bool on);
        bool test(Side side) const;
    public:
        Sides &operator|=(const Sides &c);
        bool operator==(const Sides &c) const;
        friend std::ostream& operator<<(std::ostream& os, const Sides &sides);
        operator bool() const;
        operator int() const;
        int count() const;
        bool testLeft() const;
        bool testTop() const;
        bool testRight() const;
        bool testBottom() const;
        void setLeft(bool on);
        void setTop(bool on);
        void setRight(bool on);
        void setBottom(bool on);
        void clear();
        Sides flip() const;
    };
}

#endif