#pragma once

#include <libutils/Vector.h>

#include <libgraphic/svg/Arc.h>
#include <libutils/Bezier.h>

namespace Graphic
{

class SubPath
{
private:
    Vector<Vec2f> _points;
    bool _closed = false;

public:
    size_t length() const;

    Bezierf curves(size_t index) const;

    bool closed() const { return _closed; }

    Vec2f first_point() const;

    Vec2f last_point() const;

    Vec2f last_cubic_control_point() const;

    SubPath();

    SubPath(Vec2f start);

    void add(Vec2f point);

    void reset();

    void reset(Vec2f start);

    void close();

    void move_to(Vec2f point);

    void move_to_relative(Vec2f point);

    void line_to(Vec2f point);

    void line_to_relative(Vec2f point);

    void vline_to(float y);

    void vline_to_relative(float y);

    void hline_to(float x);

    void hline_to_relative(float x);

    void cubic_bezier_to(Vec2f control_point1, Vec2f control_point2, Vec2f point);

    void cubic_bezier_to_relative(Vec2f control_point1, Vec2f control_point2, Vec2f point);

    void smooth_cubic_bezier_to(Vec2f control_point, Vec2f point);

    void smooth_cubic_bezier_to_relative(Vec2f control_point, Vec2f point);

    void quad_bezier_to(Vec2f control_point, Vec2f point);

    void quad_bezier_to_relative(Vec2f control_point, Vec2f point);

    void smooth_quad_bezier_to(Vec2f point);

    void smooth_quad_bezier_to_relative(Vec2f point);

    void arc_to(float rx, float ry, float angle, int flags, Vec2f point);

    void arc_to_relative(float rx, float ry, float angle, int flags, Vec2f point);
};

} // namespace Graphic
