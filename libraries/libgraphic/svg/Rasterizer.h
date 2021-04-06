#pragma once

#include <libutils/Array.h>
#include <libgraphic/svg/EdgeList.h>

#include <libgraphic/Bitmap.h>
#include <libgraphic/svg/Paint.h>
#include <libgraphic/svg/Path.h>

namespace Graphic
{

class Rasterizer
{
private:
    static constexpr auto TOLERANCE = 0.25f;
    static constexpr auto MAX_DEPTH = 8;

    RefPtr<Bitmap> _bitmap;
    EdgeList _edges;
    Vector<Edgef> _actives_edges;
    Optional<Recti> _clip;
    Vector<uint16_t> _scanline;

    void clear();

    void flatten(const Path &path, const Trans2f &transform);

    void rasterize(Paint &paint);

public:

    Rasterizer(RefPtr<Bitmap> bitmap);

    void set_clip(Recti c);

    Recti get_clip();

    void fill(Path &path, Trans2f transform, Paint paint);
};

} // namespace Graphic
