#pragma once

#include <libmath/Vec2.h>

namespace Math
{

template <typename Scalar>
class HLine
{
private:
    Vec2<Scalar> _start;
    Scalar _length;

public:
    Vec2<Scalar> start() { return _start; }
    Scalar length() { return _length; }

    HLine(Vec2<Scalar> start, Scalar length)
        : _start{start}, _length{length}
    {
    }
};

using HLinei = HLine<int>;
using HLinef = HLine<float>;
using HLined = HLine<double>;

} // namespace Math