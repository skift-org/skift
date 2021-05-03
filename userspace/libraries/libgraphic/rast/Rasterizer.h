#pragma once

#include <libutils/Array.h>

#include <libgraphic/Bitmap.h>
#include <libgraphic/rast/EdgeList.h>
#include <libgraphic/rast/Paint.h>
#include <libgraphic/svg/Path.h>

namespace Graphic
{

struct RasterizeState
{
    Math::Vec2i origin;
    Math::Recti clip;
};

class Rasterizer
{
private:
    NONCOPYABLE(Rasterizer);
    NONMOVABLE(Rasterizer);

    static constexpr auto STATESTACK_SIZE = 32;
    static constexpr auto TOLERANCE = 0.25f;
    static constexpr auto MAX_DEPTH = 8;

    RefPtr<Bitmap> _bitmap;
    EdgeList _edges;
    Vector<Math::Edgef> _actives_edges;
    Vector<uint16_t> _scanline;

    Array<RasterizeState, STATESTACK_SIZE> _stats{};

    int _top = 0;

    void clear();

    void flatten(const Path &path, const Math::Mat3x2f &transform);

    void flatten(const EdgeList &edges, const Math::Mat3x2f &transform);

    void rasterize(Paint &paint);

public:
    Rasterizer(RefPtr<Bitmap> bitmap);

    void push();

    void pop();

    void clip(Math::Recti c);

    Math::Recti clip();

    void origin(Math::Vec2i o);

    Math::Vec2i origin();

    void fill(const Path &path, const Math::Mat3x2f &transform, Paint paint);

    void fill(const EdgeList &edges, const Math::Mat3x2f &transform, Paint paint);
};

} // namespace Graphic
