#pragma once

#include <libgraphic/Bitmap.h>
#include <libio/Reader.h>

namespace Png
{

ResultOr<RefPtr<Graphic::Bitmap>> load(IO::Reader &reader);

} // namespace Png