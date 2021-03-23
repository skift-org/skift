#include <libgraphic/Painter.h>
#include <libgraphic/svg/Rasterizer.h>

namespace Graphic
{

void Rasterizer::tessellate_cubic_bezier(BezierCurve &curve, int depth)
{
    if (depth > MAX_DEPTH)
    {
        return;
    }

    auto a = curve.start;
    auto b = curve.first_control_point;
    auto c = curve.second_contol_point;
    auto d = curve.end;

    auto delta1 = d - a;
    float delta2 = fabsf((b.x() - d.x()) * delta1.y() - (b.y() - d.y()) * delta1.x());
    float delta3 = fabsf((c.x() - d.x()) * delta1.y() - (c.y() - d.y()) * delta1.x());

    if ((delta2 + delta3) * (delta2 + delta3) <
        TOLERANCE * (delta1.x() * delta1.x() + delta1.y() * delta1.y()))
    {
        _points.push_back(d);
        return;
    }

    auto ab = (a + b) / 2;
    auto bc = (b + c) / 2;
    auto cd = (c + d) / 2;
    auto abc = (ab + bc) / 2;
    auto bcd = (bc + cd) / 2;
    auto abcd = (abc + bcd) / 2;

    BezierCurve curve_a{a, ab, abc, abcd};

    tessellate_cubic_bezier(curve_a, depth + 1);

    BezierCurve curve_b{abcd, bcd, cd, d};

    tessellate_cubic_bezier(curve_b, depth + 1);
}

// void Rasterizer::fill(Path &path, Vec2f position, Trans2f transform, Color color)
// {
// }

void Rasterizer::stroke(Painter &painter, Path &path, Vec2f position, Trans2f transform, Color color)
{
    for (size_t i = 0; i < path.subpath_count(); i++)
    {
        _points.clear();

        auto &subpath = path.subpath(i);

        _points.push_back(transform.apply(subpath.first_point()));

        for (size_t j = 0; j < subpath.length(); j++)
        {
            auto curve = subpath.curves(j);

            curve.start = transform.apply(curve.start) + position;
            curve.first_control_point = transform.apply(curve.first_control_point) + position;
            curve.second_contol_point = transform.apply(curve.second_contol_point) + position;
            curve.end = transform.apply(curve.end) + position;

            flatten(curve);
        }

        if (_points.count() > 0)
        {
            for (size_t i = 0; i < _points.count() - 1; i += 1)
            {
                painter.draw_line(_points[i], _points[i + 1], color);
            }
        }
    }
}
} // namespace Graphic
