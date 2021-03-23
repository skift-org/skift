#pragma once

#include <libutils/Trans2.h>

#include <libgraphic/Bitmap.h>
#include <libgraphic/svg/Path.h>

class Painter;

namespace Graphic
{

class Rasterizer
{
private:
    Vector<Vec2f> _points;

public:
    static constexpr auto TOLERANCE = 0.25f;
    static constexpr auto MAX_DEPTH = 8;

    Rasterizer()
    {
    }

    void tessellate_cubic_bezier(BezierCurve &curve, int depth);

    void flatten(BezierCurve &curve) { tessellate_cubic_bezier(curve, 0); }

    void rasterize();

    void fill(Path &path, Vec2f position, Trans2f transform, Color color);

    void stroke(Painter &painter, Path &path, Vec2f position, Trans2f transform, Color color);
};

} // namespace Graphic
