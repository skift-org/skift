#pragma once

#include <karm-sys/mmap.h>

#include "font.h"
#include "image.h"

namespace Karm::Media {

/* --- Font loading --------------------------------------------------------- */

Res<Strong<Fontface>> loadFontface(Sys::Mmap &&map);

Res<Strong<Fontface>> loadFontface(Sys::Url url);

Res<Strong<Fontface>> loadFontfaceOrFallback(Sys::Url url);

Res<Font> loadFont(f64 size, Sys::Url url);

Res<Font> loadFontOrFallback(f64 size, Sys::Url url);

/* --- Image loading -------------------------------------------------------- */

Res<Image> loadImage(Sys::Mmap &&map);

Res<Image> loadImage(Sys::Url url);

Res<Image> loadImageOrFallback(Sys::Url url);

} // namespace Karm::Media
