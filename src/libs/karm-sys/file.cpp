#include <embed-sys/sys.h>

#include "file.h"

namespace Karm::Sys {

Result<File> File::create(Path path) {
    return File{try$(Embed::createFile(path)), path};
}

Result<File> File::open(Path path) {
    auto fd = try$(Embed::openFile(path));
    return File{fd, path};
}

} // namespace Karm::Sys
