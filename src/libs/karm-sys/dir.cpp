#include <embed-sys/sys.h>

#include "dir.h"

namespace Karm::Sys {

Res<Dir> Dir::open(Url url) {
    auto entries = try$(Embed::readDir(url));
    sort(entries, [](auto const &lhs, auto const &rhs) {
        return cmp(lhs.name, rhs.name);
    });
    return Ok(Dir{entries, url});
}

} // namespace Karm::Sys
