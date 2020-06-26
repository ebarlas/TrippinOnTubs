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
    };

    Sides &Sides::operator|=(const Sides &s) {
        sides |= s.sides;
        return *this;
    }

    Sides::operator bool() const {
        return sides.any();
    }

    Sides::operator int() const {
        return sides.count();
    }

    int Sides::count() const {
        return sides.count();
    }

    void Sides::set(Sides::Side side, bool on) {
        sides.set(side, on);
    }

    bool Sides::test(Sides::Side side) const {
        return sides.test(side);
    }

    bool Sides::testLeft() const {
        return test(Side::left);
    }

    bool Sides::testTop() const {
        return test(Side::top);
    }

    bool Sides::testRight() const {
        return test(Side::right);
    }

    bool Sides::testBottom() const {
        return test(Side::bottom);
    }

    void Sides::setLeft(bool on) {
        set(Side::left, on);
    }

    void Sides::setTop(bool on) {
        set(Side::top, on);
    }

    void Sides::setRight(bool on) {
        set(Side::right, on);
    }

    void Sides::setBottom(bool on) {
        set(Side::bottom, on);
    }
}

#endif