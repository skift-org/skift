#pragma once

#include "font.h"
#include "image.h"

namespace Karm::Media {

Result<Strong<Fontface>> loadFontface(Str path);

Result<Font> loadFont(double size, Str path);

Result<Image> loadImage(Str path);

} // namespace Karm::Media
