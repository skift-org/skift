#pragma once

#include <libutils/Vec2.h>

template <typename Scalar>
class Edge
{
private:
    Vec2<Scalar> _start;
    Vec2<Scalar> _end;

public:
    Scalar min_x() const { return MIN(_start.x(), _end.x()); }
    Scalar min_y() const { return MIN(_start.y(), _end.y()); }

    Scalar max_x() const { return MAX(_start.x(), _end.x()); }
    Scalar max_y() const { return MAX(_start.y(), _end.y()); }

    Vec2<Scalar> start() const { return _start; }
    Vec2<Scalar> end() const { return _end; }
    Vec2<Scalar> center() const { return (_start + _end) / 2; }

    Vec2<Scalar> intersect_y(Scalar y)
    {
        Scalar x = _start.x() + (y - _start.y()) / (_end.y() - _start.y()) * (_end.x() - _start.x());
        return {x, y};
    }

    Edge(Vec2<Scalar> start, Vec2<Scalar> end)
    {
        _start = end;
        _end = start;
    }
};

using Edgei = Edge<int>;
using Edgef = Edge<float>;
using Edged = Edge<double>;
