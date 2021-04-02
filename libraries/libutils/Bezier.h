#pragma once

#include <libutils/Vec2.h>

template <typename Scalar>
class Bezier
{
private:
    Vec2<Scalar> _start;
    Vec2<Scalar> _cp1;
    Vec2<Scalar> _cp2;
    Vec2<Scalar> _end;

public:
    const Vec2<Scalar> &start() const { return _start; }
    const Vec2<Scalar> &cp1() const { return _cp1; }
    const Vec2<Scalar> &cp2() const { return _cp2; }
    const Vec2<Scalar> &end() const { return _end; }

    Bezier(Vec2<Scalar> start,
           Vec2<Scalar> cp1,
           Vec2<Scalar> cp2,
           Vec2<Scalar> end)
        : _start{start},
          _cp1{cp1},
          _cp2{cp2},
          _end{end}
    {
    }
};

using Bezieri = Bezier<int>;
using Bezierf = Bezier<float>;
using Bezierd = Bezier<double>;
