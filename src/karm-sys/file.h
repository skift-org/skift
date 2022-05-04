#pragma once

#include <karm-base/rc.h>

#include "fd.h"
#include "path.h"

namespace Karm::Sys {

struct File {
    Fd _fd;
    Path _path;

    static Result<File> create(Path &path) {
        return File{try$(Fd::create(path)), path};
    }

    static Result<File> open(Path &path);
};

} // namespace Karm::Sys
