#pragma once

#include <libmath/Edge.h>
#include <libmath/Mat3x2.h>
#include <libmath/Rect.h>
#include <libutils/Vector.h>

namespace Graphic
{

struct EdgeList
{
private:
    static constexpr auto TOLERANCE = 0.25f;
    static constexpr auto MAX_DEPTH = 8;

    Optional<Math::Vec2f> _last;
    Vector<Math::Edgef> _edges;

    bool _has_min_max = false;
    Math::Vec2f _min;
    Math::Vec2f _max;

    void flatten(Math::CubicBezierf curve, int depth)
    {
        if (depth > MAX_DEPTH)
        {
            return;
        }

        auto a = curve.start();
        auto b = curve.cp1();
        auto c = curve.cp2();
        auto d = curve.end();

        auto delta1 = d - a;
        float delta2 = fabsf((b.x() - d.x()) * delta1.y() - (b.y() - d.y()) * delta1.x());
        float delta3 = fabsf((c.x() - d.x()) * delta1.y() - (c.y() - d.y()) * delta1.x());

        if ((delta2 + delta3) * (delta2 + delta3) <
            TOLERANCE * (delta1.x() * delta1.x() + delta1.y() * delta1.y()))
        {
            append(d);
            return;
        }

        auto ab = (a + b) / 2;
        auto bc = (b + c) / 2;
        auto cd = (c + d) / 2;
        auto abc = (ab + bc) / 2;
        auto bcd = (bc + cd) / 2;
        auto abcd = (abc + bcd) / 2;

        Math::CubicBezierf curve_a{a, ab, abc, abcd};

        flatten(curve_a, depth + 1);

        Math::CubicBezierf curve_b{abcd, bcd, cd, d};

        flatten(curve_b, depth + 1);
    }

public:
    Math::Recti bound() const
    {
        return Math::Recti::from_two_point(_min, _max + Math::Vec2i{1});
    }

    Vector<Math::Edgef> const &edges() const
    {
        return _edges;
    }

    void begin()
    {
        _last.clear();
    }

    void end()
    {
        if (_last.present())
        {
            append(_last.unwrap());
            _last.clear();
        }
    }

    void append(Math::Vec2f p)
    {
        if (!_last.present())
        {
            _last = p;
        }
        else
        {
            _edges.push_back({_last.unwrap(), p});

            _last = p;
        }

        if (_has_min_max)
        {
            _min = {MIN(_min.x(), p.x()), MIN(_min.y(), p.y())};
            _max = {MAX(_max.x(), p.x()), MAX(_max.y(), p.y())};
        }
        else
        {
            _min = p;
            _max = p;
            _has_min_max = true;
        }
    }

    void append(Math::CubicBezierf curve)
    {
        begin();
        append(curve.start());
        flatten(curve, 0);
        end();
    }

    void clear()
    {
        _has_min_max = false;

        _min = Math::Vec2f::zero();
        _max = Math::Vec2f::zero();

        _last.clear();
        _edges.clear();
    }
};

} // namespace Graphic
