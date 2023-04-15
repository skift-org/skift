#pragma once

#include <karm-sys/mmap.h>

#include "font.h"
#include "image.h"

namespace Karm::Media {

Res<Strong<Fontface>> loadFontface(Sys::Mmap &&map);

Res<Strong<Fontface>> loadFontface(Str path);

Res<Font> loadFont(f64 size, Str path);

Res<Image> loadImage(Sys::Mmap &&map);

Res<Image> loadImage(Str path);

} // namespace Karm::Media
