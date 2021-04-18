#pragma once

#include <libmath/Vec2.h>

namespace Math
{

template <typename Scalar>
class CubicBezier
{
private:
    Math::Vec2<Scalar> _start;
    Math::Vec2<Scalar> _cp1;
    Math::Vec2<Scalar> _cp2;
    Math::Vec2<Scalar> _end;

public:
    const Math::Vec2<Scalar> &start() const { return _start; }
    const Math::Vec2<Scalar> &cp1() const { return _cp1; }
    const Math::Vec2<Scalar> &cp2() const { return _cp2; }
    const Math::Vec2<Scalar> &end() const { return _end; }

    CubicBezier(Math::Vec2<Scalar> start,
                Math::Vec2<Scalar> cp1,
                Math::Vec2<Scalar> cp2,
                Math::Vec2<Scalar> end)
        : _start{start},
          _cp1{cp1},
          _cp2{cp2},
          _end{end}
    {
    }
};

using CubicBezieri = CubicBezier<int>;
using CubicBezierf = CubicBezier<float>;
using CubicBezierd = CubicBezier<double>;

} // namespace Math