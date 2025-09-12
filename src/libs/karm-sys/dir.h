#pragma once

import Karm.Core;
import Karm.Ref;

#include "stat.h"

namespace Karm::Sys {

struct DirEntry {
    String name;
    Type type;

    bool hidden() const {
        return name[0] == '.';
    }
};

struct Dir {
    Vec<DirEntry> _entries;
    Ref::Url _url;

    static Res<Dir> open(Ref::Url url);

    static Res<> create(Ref::Url url);

    static Res<Dir> openOrCreate(Ref::Url url);

    auto const& entries() const { return _entries; }

    auto const& path() const { return _url; }
};

} // namespace Karm::Sys
