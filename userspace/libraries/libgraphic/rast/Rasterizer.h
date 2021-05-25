#pragma once

#include <libgraphic/Bitmap.h>
#include <libgraphic/rast/EdgeList.h>
#include <libgraphic/rast/Paint.h>
#include <libgraphic/rast/TransformStack.h>
#include <libgraphic/svg/Path.h>

namespace Graphic
{

struct RasterizeState
{
    Math::Vec2i origin;
    Math::Recti clip;
};

struct Rasterizer
{
private:
    NONCOPYABLE(Rasterizer);
    NONMOVABLE(Rasterizer);

    static constexpr auto TOLERANCE = 0.25f;
    static constexpr auto MAX_DEPTH = 8;

    Bitmap &_bitmap;
    TransformStack &_stack;

    EdgeList _edges;
    Vector<Math::Edgef> _actives_edges;
    Vector<uint16_t> _scanline;

    void clear();

    void flatten(const Path &path, const Math::Mat3x2f &transform);

    void flatten(const EdgeList &edges, const Math::Mat3x2f &transform);

    void rasterize(Paint &paint);

public:
    Bitmap &bitmap() const { return _bitmap; }

    Rasterizer(Bitmap &bitmap, TransformStack &stack);

    void fill(const Path &path, const Math::Mat3x2f &transform, Paint paint);

    void fill(const EdgeList &edges, const Math::Mat3x2f &transform, Paint paint);
};

} // namespace Graphic
