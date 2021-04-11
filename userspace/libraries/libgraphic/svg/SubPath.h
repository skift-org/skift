#pragma once

#include <libutils/Vector.h>

#include <libgraphic/svg/Arc.h>
#include <libutils/Bezier.h>

namespace Graphic
{

class SubPath
{
private:
    Vector<Math::Vec2f> _points;
    bool _closed = false;

public:
    size_t length() const;

    Bezierf curves(size_t index) const;

    bool closed() const { return _closed; }

    Math::Vec2f first_point() const;

    Math::Vec2f last_point() const;

    Math::Vec2f last_cubic_control_point() const;

    SubPath();

    SubPath(Math::Vec2f start);

    void add(Math::Vec2f point);

    void reset();

    void reset(Math::Vec2f start);

    void close();

    void move_to(Math::Vec2f point);

    void move_to_relative(Math::Vec2f point);

    void line_to(Math::Vec2f point);

    void line_to_relative(Math::Vec2f point);

    void vline_to(float y);

    void vline_to_relative(float y);

    void hline_to(float x);

    void hline_to_relative(float x);

    void cubic_bezier_to(Math::Vec2f control_point1, Math::Vec2f control_point2, Math::Vec2f point);

    void cubic_bezier_to_relative(Math::Vec2f control_point1, Math::Vec2f control_point2, Math::Vec2f point);

    void smooth_cubic_bezier_to(Math::Vec2f control_point, Math::Vec2f point);

    void smooth_cubic_bezier_to_relative(Math::Vec2f control_point, Math::Vec2f point);

    void quad_bezier_to(Math::Vec2f control_point, Math::Vec2f point);

    void quad_bezier_to_relative(Math::Vec2f control_point, Math::Vec2f point);

    void smooth_quad_bezier_to(Math::Vec2f point);

    void smooth_quad_bezier_to_relative(Math::Vec2f point);

    void arc_to(float rx, float ry, float angle, int flags, Math::Vec2f point);

    void arc_to_relative(float rx, float ry, float angle, int flags, Math::Vec2f point);
};

} // namespace Graphic
