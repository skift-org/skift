#pragma once

#include <karm-base/res.h>
#include <karm-base/vec.h>
#include <karm-mime/url.h>

namespace Karm::Sys {

struct DirEntry {
    String name;
    bool isDir;
};

struct Dir {
    Vec<DirEntry> _entries;
    Mime::Url _url;

    static Res<Dir> open(Mime::Url url);

    auto const &entries() const { return _entries; }

    auto const &path() const { return _url; }
};

} // namespace Karm::Sys
