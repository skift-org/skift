#pragma once

#include <libutils/Rect.h>
#include <libutils/Scanner.h>
#include <libutils/Vector.h>

#include <libgraphic/svg/Arc.h>
#include <libgraphic/svg/SubPath.h>

namespace Graphic
{

class Path
{
private:
    Vector<SubPath> _subpath;
    bool _subpath_ended = true;

    SubPath &current()
    {
        if (_subpath_ended)
        {
            begin_subpath();
        }

        return _subpath[_subpath.count() - 1];
    }

    SubPath &current_or_begin()
    {
        if (_subpath_ended)
        {
            begin_subpath(current().first_point());
        }

        return _subpath[_subpath.count() - 1];
    }

public:
    static Path parse(const char *str)
    {
        StringScanner scan{str, strlen(str)};
        return parse(scan);
    }

    static Path parse(Scanner &scan);

    const SubPath &subpath(size_t index) const
    {
        return _subpath[index];
    }

    size_t subpath_count() const
    {
        return _subpath.count();
    }

    Path()
    {
    }

    void begin_subpath()
    {
        begin_subpath({0, 0});
    }

    void begin_subpath(Vec2f point)
    {
        if (!_subpath_ended)
        {
            end_subpath();
        }

        SubPath subpath{point};
        _subpath.push_back(move(subpath));
        _subpath_ended = false;
    }

    void begin_subpath_relative()
    {
        begin_subpath_relative({0, 0});
    }

    void begin_subpath_relative(Vec2f point)
    {
        if (_subpath.count() > 0)
        {
            begin_subpath(current().last_point() + point);
        }
        else
        {
            begin_subpath(point);
        }
    }

    void reset_subpath()
    {
        current().reset();
    }

    void reset_subpath(Vec2f start)
    {
        current().reset(start);
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

    void move_to(Vec2f point)
    {
        current().move_to(point);
    }

    void move_to_relative(Vec2f point)
    {
        current().move_to_relative(point);
    }

    void line_to(Vec2f point)
    {
        current_or_begin().line_to(point);
    }

    void line_to_relative(Vec2f point)
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

    void cubic_bezier_to(Vec2f control_point1, Vec2f control_point2, Vec2f point)
    {
        current_or_begin().cubic_bezier_to(control_point1, control_point2, point);
    }

    void cubic_bezier_to_relative(Vec2f control_point1, Vec2f control_point2, Vec2f point)
    {
        current_or_begin().cubic_bezier_to_relative(control_point1, control_point2, point);
    }

    void smooth_cubic_bezier_to(Vec2f control_point, Vec2f point)
    {
        current_or_begin().smooth_cubic_bezier_to(control_point, point);
    }

    void smooth_cubic_bezier_to_relative(Vec2f control_point, Vec2f point)
    {
        current_or_begin().smooth_cubic_bezier_to_relative(control_point, point);
    }

    void quad_bezier_to(Vec2f control_point, Vec2f point)
    {
        current_or_begin().quad_bezier_to(control_point, point);
    }

    void quad_bezier_to_relative(Vec2f control_point, Vec2f point)
    {
        current_or_begin().quad_bezier_to_relative(control_point, point);
    }

    void smooth_quad_bezier_to(Vec2f point)
    {
        current_or_begin().smooth_quad_bezier_to(point);
    }

    void smooth_quad_bezier_to_relative(Vec2f point)
    {
        current_or_begin().smooth_quad_bezier_to_relative(point);
    }

    void arc_to(float rx, float ry, float angle, int flags, Vec2f point)
    {
        current_or_begin().arc_to(rx, ry, angle, flags, point);
    }

    void arc_to_relative(float rx, float ry, float angle, int flags, Vec2f point)
    {
        current_or_begin().arc_to_relative(rx, ry, angle, flags, point);
    }
};

} // namespace Graphic
