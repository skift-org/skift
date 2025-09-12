#include "dir.h"

#include "_embed.h"
#include "proc.h"

namespace Karm::Sys {

Res<Dir> Dir::open(Ref::Url url) {
    if (url.scheme != "bundle")
        try$(ensureUnrestricted());

    auto entries = try$(_Embed::readDir(url));
    sort(entries, [](auto const& lhs, auto const& rhs) {
        return lhs.name <=> rhs.name;
    });
    return Ok(Dir{entries, url});
}

Res<> Dir::create(Ref::Url url) {
    try$(ensureUnrestricted());
    return _Embed::createDir(url);
}

Res<Dir> Dir::openOrCreate(Ref::Url url) {
    try$(ensureUnrestricted());
    auto entries = try$(_Embed::readDirOrCreate(url));
    sort(entries, [](auto const& lhs, auto const& rhs) {
        return lhs.name <=> rhs.name;
    });
    return Ok(Dir{entries, url});
}

} // namespace Karm::Sys
