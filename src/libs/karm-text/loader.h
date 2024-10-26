#pragma once

#include <karm-sys/mmap.h>

#include "font.h"

namespace Karm::Text {

Res<Strong<Fontface>> loadFontface(Sys::Mmap &&map);

Res<Strong<Fontface>> loadFontface(Mime::Url url);

Res<Strong<Fontface>> loadFontfaceOrFallback(Mime::Url url);

Res<Font> loadFont(f64 size, Mime::Url url);

Res<Font> loadFontOrFallback(f64 size, Mime::Url url);

} // namespace Karm::Text
