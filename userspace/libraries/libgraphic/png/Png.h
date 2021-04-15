#pragma once

#include <libgraphic/Bitmap.h>
#include <libio/Reader.h>

namespace Graphic::Png
{
ResultOr<RefPtr<Bitmap>> load(IO::Reader &reader);
} // namespace Graphic::Svg