#pragma once

#include <karm-base/result.h>
#include <karm-base/vec.h>
#include <karm-meta/utils.h>

#include "path.h"

namespace Karm::Sys {

struct DirEntry {
    String name;
    bool isDir;
};

struct Dir {
    Vec<DirEntry> _entries;
    Path _path;

    static Result<Dir> open(Path path);

    auto const &entries() const { return _entries; }

    auto const &path() const { return _path; }
};

} // namespace Karm::Sys
