#pragma once

#include <libgraphic/font/Font.h>
#include <libio/Reader.h>

namespace Graphic::Font
{
ResultOr<RefPtr<Font>> load(IO::Reader &reader);
} // namespace Graphic::Font