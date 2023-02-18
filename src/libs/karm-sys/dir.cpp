#include <embed-sys/sys.h>

#include "dir.h"

namespace Karm::Sys {

Res<Dir> Dir::open(Path path) {
    auto entries = try$(Embed::readDir(path));
    sort(entries, [](auto const &lhs, auto const &rhs) {
        return cmp(lhs.name, rhs.name);
    });
    return Ok(Dir{entries, path});
}

} // namespace Karm::Sys
