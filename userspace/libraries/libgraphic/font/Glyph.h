#pragma once

#include <libgraphic/font/Font.h>
#include <libio/Reader.h>
#include <libmath/Rect.h>

namespace Graphic::Font
{
struct Glyph
{
  int advance;
  Math::Recti bounds;
};
} // namespace Graphic::Font