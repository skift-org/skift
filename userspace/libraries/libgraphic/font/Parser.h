#pragma once

#include <libgraphic/font/FontFace.h>
#include <libio/Reader.h>

namespace Graphic::Font
{
ResultOr<RefPtr<FontFace>> load(IO::Reader &reader);
} // namespace Graphic::Font