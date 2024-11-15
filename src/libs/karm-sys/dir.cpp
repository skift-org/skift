#include "dir.h"

#include "_embed.h"

#include "proc.h"

namespace Karm::Sys {

Res<Dir> Dir::open(Mime::Url url) {
    try$(ensureUnrestricted());

    auto entries = try$(_Embed::readDir(url));
    sort(entries, [](auto const &lhs, auto const &rhs) {
        return lhs.name <=> rhs.name;
    });
    return Ok(Dir{entries, url});
}

} // namespace Karm::Sys
