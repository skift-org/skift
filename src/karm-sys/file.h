#pragma once

#include <embed/sys.h>
#include <karm-base/rc.h>

#include "fd.h"
#include "path.h"

namespace Karm::Sys {

struct File {
    Base::Strong<Fd> _fd;
    Path _path;

    static Result<File> create(Path &path) {
        return File{try$(Embed::createFd(path)), path};
    }

    static Result<File> open(Path &path);
};

} // namespace Karm::Sys
