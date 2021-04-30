#pragma once

#include <libio/Reader.h>
#include <libmath/Rect.h>
#include <libgraphic/rast/EdgeList.h>

namespace Graphic::Font
{
struct Glyph
{
    int advance;
    Math::Recti bounds;
    EdgeList edges;
};
} // namespace Graphic::Font