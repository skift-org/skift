#include <libutils/Trans2.h>

#include <libgraphic/svg/SubPath.h>
#include <libtest/AssertLowerThan.h>

namespace Graphic
{

size_t SubPath::length() const
{
    return (_points.count() - 1) / 3;
}

BezierCurve SubPath::curves(size_t index) const
{
    assert_lower_than(index, length());

    size_t i = index * 3;
    return {_points[i], _points[i + 1], _points[i + 2], _points[i + 3]};
}

Vec2f SubPath::first_point() const
{
    if (_points.count() > 0)
    {
        return _points[0];
    }
    else
    {
        return Vec2f::zero();
    }
}

Vec2f SubPath::last_point() const
{
    if (_points.count() > 0)
    {
        return _points[_points.count() - 1];
    }
    else
    {
        return Vec2f::zero();
    }
}

Vec2f SubPath::last_cubic_control_point() const
{
    if (_points.count() > 1)
    {
        return _points[_points.count() - 2];
    }
    else
    {
        return Vec2f::zero();
    }
}

SubPath::SubPath()
{
    add({0, 0});
}

SubPath::SubPath(Vec2f start)
{
    add(start);
}

void SubPath::add(Vec2f point)
{
    _points.push_back(point);
}

void SubPath::reset()
{
    reset({0, 0});
}

void SubPath::reset(Vec2f start)
{
    _closed = false;
    _points.clear();
    add(start);
}

void SubPath::close()
{
    _closed = true;
    line_to(first_point());
}

void SubPath::move_to(Vec2f point)
{
    if (_points.count() == 1)
    {
        _points[_points.count() - 1] = point;
    }
    else
    {
        line_to(point);
    }
}

void SubPath::move_to_relative(Vec2f point)
{
    move_to(last_point() + point);
}

void SubPath::line_to(Vec2f point)
{
    auto previous = last_point();
    auto delta = point - previous;

    add(previous + (delta / 3));
    add(point - (delta / 3));
    add(point);
}

void SubPath::line_to_relative(Vec2f point)
{
    line_to(last_point() + point);
}

void SubPath::vline_to(float y)
{
    line_to({last_point().x(), y});
}

void SubPath::vline_to_relative(float y)
{
    vline_to(last_point().y() + y);
}

void SubPath::hline_to(float x)
{
    line_to({x, last_point().y()});
}

void SubPath::hline_to_relative(float x)
{
    hline_to(last_point().x() + x);
}

void SubPath::cubic_bezier_to(Vec2f control_point1, Vec2f control_point2, Vec2f point)
{
    add(control_point1);
    add(control_point2);
    add(point);
}

void SubPath::cubic_bezier_to_relative(Vec2f control_point1, Vec2f control_point2, Vec2f point)
{
    cubic_bezier_to(
        last_point() + control_point1,
        last_point() + control_point2,
        last_point() + point);
}

void SubPath::smooth_cubic_bezier_to(Vec2f control_point, Vec2f point)
{
    auto cp = last_cubic_control_point() * 2 - last_point();
    cubic_bezier_to(cp, control_point, point);
}

void SubPath::smooth_cubic_bezier_to_relative(Vec2f control_point, Vec2f point)
{
    smooth_cubic_bezier_to(last_point() + control_point, last_point() + point);
}

void SubPath::quad_bezier_to(Vec2f control_point, Vec2f point)
{
    if (_points.count() > 0)
    {
        auto previous = last_point();

        auto cp1 = previous + (control_point - previous) * (2.0f / 3.0f);
        auto cp2 = point + (control_point - point) * (2.0f / 3.0f);

        cubic_bezier_to(cp1, cp2, point);
    }
}

void SubPath::quad_bezier_to_relative(Vec2f control_point, Vec2f point)
{
    quad_bezier_to(
        last_point() + control_point,
        last_point() + point);
}

void SubPath::smooth_quad_bezier_to(Vec2f point)
{
    auto cp = last_cubic_control_point() * 2 - last_point();
    quad_bezier_to(cp, point);
}

void SubPath::smooth_quad_bezier_to_relative(Vec2f point)
{
    smooth_quad_bezier_to(last_point() + point);
}

void SubPath::arc_to(float rx, float ry, float angle, int flags, Vec2f point)
{
    auto pow2 = [](auto x) { return x * x; };

    // Ported from canvg (https://code.google.com/p/canvg/)
    float x1 = last_point().x(); // start point
    float y1 = last_point().y();
    float x2 = point.x();
    float y2 = point.y();

    float dx = x1 - x2;
    float dy = y1 - y2;
    float d = sqrtf(dx * dx + dy * dy);

    if (d < 1e-6f || rx < 1e-6f || ry < 1e-6f)
    {
        // The arc degenerates to a line
        line_to(point);
        return;
    }

    float rotx = angle / 180.0f * PI; // x rotation angle
    float sinrx = sinf(rotx);
    float cosrx = cosf(rotx);

    // Convert to center point parameterization.
    // http://www.w3.org/TR/SVG11/implnote.html#ArcImplementationNotes

    // 1) Compute x1', y1'
    float x1p = cosrx * dx / 2.0f + sinrx * dy / 2.0f;
    float y1p = -sinrx * dx / 2.0f + cosrx * dy / 2.0f;

    d = pow2(x1p) / pow2(rx) + pow2(y1p) / pow2(ry);

    if (d > 1)
    {
        d = sqrtf(d);
        rx *= d;
        ry *= d;
    }

    // 2) Compute cx', cy'
    float sa = pow2(rx) * pow2(ry) - pow2(rx) * pow2(y1p) - pow2(ry) * pow2(x1p);
    float sb = pow2(rx) * pow2(y1p) + pow2(ry) * pow2(x1p);

    if (sa < 0.0f)
    {
        sa = 0.0f;
    }

    float s = 0.0f;

    if (sb > 0.0f)
    {
        s = sqrtf(sa / sb);
    }

    bool fa = (flags & Arc::LARGE) == Arc::LARGE; // Large arc
    bool fs = (flags & Arc::SWEEP) == Arc::SWEEP; // Sweep direction

    if (fa == fs)
    {
        s = -s;
    }

    float cxp = s * rx * y1p / ry;
    float cyp = s * -ry * x1p / rx;

    // 3) Compute cx,cy from cx',cy'
    float cx = cosrx * cxp - sinrx * cyp + (x1 + x2) / 2.0f;
    float cy = sinrx * cxp + cosrx * cyp + (y1 + y2) / 2.0f;

    // 4) Calculate theta1, and delta theta.
    Vec2f u{(x1p - cxp) / rx, (y1p - cyp) / ry};
    Vec2f v{(-x1p - cxp) / rx, (-y1p - cyp) / ry};

    float a1 = Vec2f{1, 0}.angle_with(u); // Initial angle
    float da = u.angle_with(v);

    if (!fs && da > 0)
    {
        da -= 2 * PI;
    }
    else if (fs && da < 0)
    {
        da += 2 * PI;
    }

    // Approximate the arc using cubic spline segments.
    Trans2f t{
        cosrx,
        sinrx,
        -sinrx,
        cosrx,
        cx,
        cy,
    };

    // Split arc into max 90 degree segments.
    // The loop assumes an iteration per end point (including start and end), this +1.
    int ndivs = (int)(fabsf(da) / (PI * 0.5f) + 1.0f);
    float hda = (da / (float)ndivs) / 2.0f;
    float kappa = fabsf(4.0f / 3.0f * (1.0f - cosf(hda)) / sinf(hda));

    if (da < 0.0f)
    {
        kappa = -kappa;
    }

    Vec2f current = Vec2f::zero();
    Vec2f ptan = Vec2f::zero();

    for (int i = 0; i <= ndivs; i++)
    {
        float a = a1 + da * (i / (float)ndivs);

        dx = cosf(a);
        dy = sinf(a);

        auto p = t.apply({dx * rx, dy * ry});
        auto tan = t.apply_no_translation({-dy * rx * kappa, dx * ry * kappa});

        if (i > 0)
        {
            cubic_bezier_to(current + ptan, p - tan, p);
        }

        current = p;
        ptan = tan;
    }

    line_to(point);
}

void SubPath::arc_to_relative(float rx, float ry, float angle, int flags, Vec2f point)
{
    arc_to(rx, ry, angle, flags, last_point() + point);
}

} // namespace Graphic
