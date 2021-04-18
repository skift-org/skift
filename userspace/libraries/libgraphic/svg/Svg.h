#pragma once

#include <libgraphic/Bitmap.h>
#include <libio/Reader.h>

namespace Graphic::Svg
{

ResultOr<RefPtr<Bitmap>> render(IO::Reader &reader, int resolution);

} // namespace Graphic::Svg
