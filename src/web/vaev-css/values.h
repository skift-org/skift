#pragma once

#include <vaev-base/color.h>
#include <vaev-base/display.h>
#include <vaev-base/insets.h>
#include <vaev-base/length.h>
#include <vaev-base/sizing.h>

#include "parser.h"

namespace Vaev::Css {

Res<Color> parseColor(Cursor<Sst> &c);

Res<Display> parseDisplay(Cursor<Sst> &c);

Res<Length> parseLength(Cursor<Sst> &c);

Res<MarginWidth> parseMarginWidth(Cursor<Sst> &c);

Res<Percent> parsePercentage(Cursor<Sst> &c);

Res<PercentOr<Length>> parseLengthOrPercentage(Cursor<Sst> &c);

Res<Size> parseSize(Cursor<Sst> &c);

} // namespace Vaev::Css
