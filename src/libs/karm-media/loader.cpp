
#include <karm-sys/file.h>
#include <karm-sys/mmap.h>

#include "font-ttf.h"
#include "loader.h"

namespace Karm::Media {

Result<Strong<Font>> loadFont(Str path) {
    auto file = try$(Sys::File::open(path));
    auto map = try$(Sys::mmap().map(file));
    return Strong<Font>{try$(TtfFont::load(std::move(map)))};
}

} // namespace Karm::Media
