#pragma once

#include <karm-base/res.h>
#include <karm-base/vec.h>
#include <url/url.h>

namespace Karm::Sys {

struct DirEntry {
    String name;
    bool isDir;
};

struct Dir {
    Vec<DirEntry> _entries;
    Url::Url _url;

    static Res<Dir> open(Url::Url url);

    auto const &entries() const { return _entries; }

    auto const &path() const { return _url; }
};

} // namespace Karm::Sys
