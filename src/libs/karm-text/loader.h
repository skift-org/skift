#pragma once

#include <karm-sys/mmap.h>

#include "font.h"

namespace Karm::Text {

Res<Rc<Fontface>> loadFontface(Sys::Mmap&& map);

Res<Rc<Fontface>> loadFontface(Mime::Url url);

Res<Rc<Fontface>> loadFontfaceOrFallback(Mime::Url url);

Res<Font> loadFont(f64 size, Mime::Url url);

Res<Font> loadFontOrFallback(f64 size, Mime::Url url);

} // namespace Karm::Text
