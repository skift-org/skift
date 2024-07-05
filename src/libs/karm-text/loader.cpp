
#include <karm-sys/file.h>
#include <karm-sys/mmap.h>

#include "loader.h"
#include "ttf.h"

namespace Karm::Text {

Res<Strong<Fontface>> loadFontface(Sys::Mmap &&map) {
    return Ok(try$(TtfFontface::load(std::move(map))));
}

Res<Strong<Fontface>> loadFontface(Mime::Url url) {
    auto file = try$(Sys::File::open(url));
    auto map = try$(Sys::mmap().map(file));
    return loadFontface(std::move(map));
}

Res<Strong<Fontface>> loadFontfaceOrFallback(Mime::Url url) {
    if (auto result = loadFontface(url); result) {
        return result;
    }
    return Ok(Fontface::fallback());
}

Res<Font> loadFont(f64 size, Mime::Url url) {
    return Ok(Font{
        .fontface = try$(loadFontface(url)),
        .fontsize = size,
    });
}

Res<Font> loadFontOrFallback(f64 size, Mime::Url url) {
    if (auto result = loadFont(size, url); result) {
        return result;
    }
    return Ok(Font::fallback());
}

} // namespace Karm::Text
