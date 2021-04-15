#pragma once

#include <abi/Time.h>

#include <libgraphic/Bitmap.h>
#include <libgraphic/png/PngCommon.h>
#include <libio/MemoryWriter.h>
#include <libio/Reader.h>
#include <libutils/Vector.h>

namespace Graphic::Svg
{
ResultOr<Bitmap> render(IO::Reader &reader, int resolution);
} // namespace Graphic::Svg