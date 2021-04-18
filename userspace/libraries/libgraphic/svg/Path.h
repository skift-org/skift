#pragma once

#include <libmath/Rect.h>
#include <libutils/Scanner.h>
#include <libutils/Vector.h>

#include <libgraphic/svg/SubPath.h>

namespace Graphic
{

class Path
{
private:
    Vector<SubPath> _subpaths;
    bool _subpath_ended = true;

    SubPath &current()
    {
        if (_subpaths.count() == 0)
        {
            begin_subpath();
        }

        return _subpaths[_subpaths.count() - 1];
    }

    SubPath &current_or_begin()
    {
        if (_subpath_ended)
        {
            begin_subpath(current().first_point());
        }

        return _subpaths[_subpaths.count() - 1];
    }

public:
    static Path parse(const char *str);

    static Path parse(Scanner &scan);

    const Vector<SubPath> &subpaths() const
    {
        return _subpaths;
    }

    Path()
    {
    }

    void begin_subpath(Math::Vec2f point = Math::Vec2f::zero())
    {
        if (!_subpath_ended)
        {
            end_subpath();
        }

        SubPath subpath{point};
        _subpaths.push_back(move(subpath));
        _subpath_ended = false;
    }

    void begin_subpath_relative(Math::Vec2f point = Math::Vec2f::zero())
    {
        if (_subpaths.count() > 0)
        {
            begin_subpath(current().last_point() + point);
        }
        else
        {
            begin_subpath(point);
        }
    }

    void end_subpath()
    {
        _subpath_ended = true;
    }

    void close_subpath()
    {
        current().close();
        _subpath_ended = true;
    }

    void move_to(Math::Vec2f point)
    {
        current().move_to(point);
    }

    void move_to_relative(Math::Vec2f point)
    {
        current().move_to_relative(point);
    }

    void line_to(Math::Vec2f point)
    {
        current_or_begin().line_to(point);
    }

    void line_to_relative(Math::Vec2f point)
    {
        current_or_begin().line_to_relative(point);
    }

    void vline_to(float y)
    {
        current_or_begin().vline_to(y);
    }

    void vline_to_relative(float y)
    {
        current_or_begin().vline_to_relative(y);
    }

    void hline_to(float x)
    {
        current_or_begin().hline_to(x);
    }

    void hline_to_relative(float x)
    {
        current_or_begin().hline_to_relative(x);
    }

    void cubic_bezier_to(Math::Vec2f control_point1, Math::Vec2f control_point2, Math::Vec2f point)
    {
        current_or_begin().cubic_bezier_to(control_point1, control_point2, point);
    }

    void cubic_bezier_to_relative(Math::Vec2f control_point1, Math::Vec2f control_point2, Math::Vec2f point)
    {
        current_or_begin().cubic_bezier_to_relative(control_point1, control_point2, point);
    }

    void smooth_cubic_bezier_to(Math::Vec2f control_point, Math::Vec2f point)
    {
        current_or_begin().smooth_cubic_bezier_to(control_point, point);
    }

    void smooth_cubic_bezier_to_relative(Math::Vec2f control_point, Math::Vec2f point)
    {
        current_or_begin().smooth_cubic_bezier_to_relative(control_point, point);
    }

    void quad_bezier_to(Math::Vec2f control_point, Math::Vec2f point)
    {
        current_or_begin().quad_bezier_to(control_point, point);
    }

    void quad_bezier_to_relative(Math::Vec2f control_point, Math::Vec2f point)
    {
        current_or_begin().quad_bezier_to_relative(control_point, point);
    }

    void smooth_quad_bezier_to(Math::Vec2f point)
    {
        current_or_begin().smooth_quad_bezier_to(point);
    }

    void smooth_quad_bezier_to_relative(Math::Vec2f point)
    {
        current_or_begin().smooth_quad_bezier_to_relative(point);
    }

    void arc_to(float rx, float ry, float angle, int flags, Math::Vec2f point)
    {
        current_or_begin().arc_to(rx, ry, angle, flags, point);
    }

    void arc_to_relative(float rx, float ry, float angle, int flags, Math::Vec2f point)
    {
        current_or_begin().arc_to_relative(rx, ry, angle, flags, point);
    }
};

} // namespace Graphic
