#pragma once

#include <libutils/Array.h>
#include <libutils/Edge.h>
#include <libutils/Trans2.h>

#include <libgraphic/Bitmap.h>
#include <libgraphic/svg/Path.h>

namespace Graphic
{

class EdgeList
{
private:
    static constexpr auto TOLERANCE = 0.25f;
    static constexpr auto MAX_DEPTH = 8;

    Optional<Vec2f> _last;
    Vector<Edgef> _edges;

    bool _has_min_max = false;
    Vec2f _min;
    Vec2f _max;

    void flatten(Bezierf curve, int depth)
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

        Bezierf curve_a{a, ab, abc, abcd};

        flatten(curve_a, depth + 1);

        Bezierf curve_b{abcd, bcd, cd, d};

        flatten(curve_b, depth + 1);
    }

public:
    Recti bound() const
    {
        return Recti::from_two_point(_min, _max + Vec2i{1});
    }

    const Vector<Edgef> &edges()
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

    void append(Vec2f p)
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

    void append(Bezierf curve)
    {
        begin();
        append(curve.start());
        flatten(curve, 0);
        end();
    }

    void clear()
    {
        _has_min_max = false;

        _min = Vec2f::zero();
        _max = Vec2f::zero();

        _last.clear();
        _edges.clear();
    }
};

class Rasterizer
{
private:
    RefPtr<Bitmap> _bitmap;
    EdgeList _edges;
    Vector<Edgef> _actives_edges;
    Optional<Recti> _clip;
    Vector<uint16_t> _scanline;

public:
    static constexpr auto TOLERANCE = 0.25f;
    static constexpr auto MAX_DEPTH = 8;

    Rasterizer(RefPtr<Bitmap> bitmap);

    void clear();

    void set_clip(Recti c);

    Recti get_clip();

    void flatten(const Path &path, const Trans2f &transform);

    void line(float start, float end);

    void scanline(int start, int end, float y);

    void fill(Color color);

    void fill(Path &path, Trans2f transform, Color color);
};

} // namespace Graphic
