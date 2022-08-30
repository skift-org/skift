#include <embed/sys.h>

#include "dir.h"

namespace Karm::Sys {

Result<Dir> Dir::open(Path path) {
    auto entries = try$(Embed::readDir(path));
    sort(entries, [](auto const &lhs, auto const &rhs) {
        return cmp(lhs.name, rhs.name);
    });
    return Dir{entries, path};
}

} // namespace Karm::Sys
