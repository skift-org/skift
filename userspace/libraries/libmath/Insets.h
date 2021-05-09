#pragma once

#include <libmath/Vec2.h>

template <typename Scalar>
struct Insets
{
private:
    Scalar _top;
    Scalar _bottom;
    Scalar _left;
    Scalar _right;

public:
    auto top() const { return _top; }
    auto bottom() const { return _bottom; }
    auto left() const { return _left; }
    auto right() const { return _right; }

    Math::Vec2<Scalar> all()
    {
        return {left() + right(), top() + bottom()};
    }

    Insets()
        : Insets(0, 0, 0, 0)
    {
    }

    Insets(Scalar tblr)
        : Insets(tblr, tblr, tblr, tblr)
    {
    }

    Insets(Scalar tb, Scalar lr)
        : Insets(tb, tb, lr, lr)
    {
    }

    Insets(Scalar top, Scalar bottom, Scalar lr)
        : Insets(top, bottom, lr, lr)
    {
    }

    Insets(Scalar top, Scalar bottom, Scalar left, Scalar right)
        : _top(top),
          _bottom(bottom),
          _left(left),
          _right(right)
    {
    }
};

using Insetsi = Insets<int>;
using Insetsf = Insets<float>;
using Insetsd = Insets<double>;
