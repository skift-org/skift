#include <embed/sys.h>

#include "dir.h"

namespace Karm::Sys {

Result<Dir> Dir::open(Path path) {
    return Dir{try$(Embed::readDir(path)), path};
}

} // namespace Karm::Sys
