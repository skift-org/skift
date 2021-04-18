#pragma once

#include <libmath/Vec2.h>

namespace Math
{

template <typename Scalar>
class Arc
{
public:
    enum ArcFlag
    {
        LARGE = 1 << 0,
        SWEEP = 1 << 1,
    };

private:
    Vec2<Scalar> _start;
    Vec2<Scalar> _end;
    Vec2<Scalar> _radii;
    Scalar _rotation;
    int _flags;

public:
    Vec2<Scalar> start() { return _start; }
    Vec2<Scalar> end() { return _end; }
    Vec2<Scalar> radii() { return _radii; }
    Scalar rotation() { return _rotation; }
    int flags() { return _flags; }

    Arc(Vec2<Scalar> start, Vec2<Scalar> end, Vec2<Scalar> radii, Scalar rotation, int flags)
        : _start(start),
          _end(end),
          _radii(radii),
          _rotation(rotation),
          _flags(flags)
    {
    }
};

using Arci = Arc<int>;
using Arcf = Arc<float>;
using Arcd = Arc<double>;

} // namespace Math
