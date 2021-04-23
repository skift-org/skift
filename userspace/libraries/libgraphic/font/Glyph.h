#pragma once

#include <libgraphic/font/Font.h>
#include <libio/Reader.h>
#include <libmath/Rect.h>

namespace Graphic::Font
{
struct Glyph
{
    int advance;
    int ascend;
    int descend;
    Math::Recti bounds;
};
} // namespace Graphic::Font