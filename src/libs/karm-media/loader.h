#pragma once

#include <karm-sys/mmap.h>

#include "font.h"
#include "image.h"

namespace Karm::Media {

/* --- Font loading --------------------------------------------------------- */

Res<Strong<Fontface>> loadFontface(Sys::Mmap &&map);

Res<Strong<Fontface>> loadFontface(Url::Url url);

Res<Strong<Fontface>> loadFontfaceOrFallback(Url::Url url);

Res<Font> loadFont(f64 size, Url::Url url);

Res<Font> loadFontOrFallback(f64 size, Url::Url url);

/* --- Image loading -------------------------------------------------------- */

Res<Image> loadImage(Sys::Mmap &&map);

Res<Image> loadImage(Url::Url url);

Res<Image> loadImageOrFallback(Url::Url url);

} // namespace Karm::Media
