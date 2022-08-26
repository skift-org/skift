#pragma once

#include "font.h"

namespace Karm::Media {

Result<Strong<Fontface>> loadFontface(Str path);

Result<Font> loadFont(double size, Str path);

} // namespace Karm::Media
