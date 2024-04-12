#pragma once

#include <karm-sys/mmap.h>

#include "font.h"
#include "image.h"

namespace Karm::Media {

// MARK: Font loading ----------------------------------------------------------

Res<Strong<Fontface>> loadFontface(Sys::Mmap &&map);

Res<Strong<Fontface>> loadFontface(Mime::Url url);

Res<Strong<Fontface>> loadFontfaceOrFallback(Mime::Url url);

Res<Font> loadFont(f64 size, Mime::Url url);

Res<Font> loadFontOrFallback(f64 size, Mime::Url url);

// MARK: Image loading ---------------------------------------------------------

Res<Image> loadImage(Sys::Mmap &&map);

Res<Image> loadImage(Mime::Url url);

Res<Image> loadImageOrFallback(Mime::Url url);

} // namespace Karm::Media
