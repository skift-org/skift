#pragma once

#include <libmath/Vec2.h>

namespace Math
{

template <typename Scalar>
class VLine
{
private:
    Vec2<Scalar> _start;
    Scalar _length;

public:
    Vec2<Scalar> start() { return _start; }
    Scalar length() { return _length; }

    VLine(Vec2<Scalar> start, Scalar length)
        : _start{start}, _length{length}
    {
    }
};

using VLinei = VLine<int>;
using VLinef = VLine<float>;
using VLined = VLine<double>;

} // namespace Math
