#pragma once

#include <karm-base/rc.h>

#include "fd.h"
#include "path.h"

namespace Karm::Sys {

struct File {
    Fd _fd;
    Path _path;

    static auto create(Path &path) -> Result<File> {
        auto fd = Fd::create(path);
        if (!fd) {
            return fd.error();
        }
        return File{*fd, path};
    }

    static auto open(Path &path) -> Result<File>;
};

} // namespace Karm::Sys
