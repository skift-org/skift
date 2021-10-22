#pragma once

#include <libgraphic/Bitmap.h>
#include <libio/Reader.h>

namespace Graphic::Svg
{

ResultOr<Ref<Bitmap>> render(IO::Reader &reader, int resolution);

} // namespace Graphic::Svg
