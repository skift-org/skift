
#include <karm-sys/file.h>
#include <karm-sys/mmap.h>

#include "font-ttf.h"
#include "loader.h"

namespace Karm::Media {

Result<Strong<Fontface>> loadFontface(Str path) {
    auto file = try$(Sys::File::open(path));
    auto map = try$(Sys::mmap().map(file));
    Strong<Fontface> face = try$(TtfFont::load(std::move(map)));
    return face;
}

Result<Font> loadFont(double size, Str path) {
    return Font{size, try$(loadFontface(path))};
}

} // namespace Karm::Media
