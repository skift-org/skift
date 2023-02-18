#include <embed-sys/sys.h>

#include "file.h"

namespace Karm::Sys {

Res<File> File::create(Path path) {
    return Ok(File{try$(Embed::createFile(path)), path});
}

Res<File> File::open(Path path) {
    auto fd = try$(Embed::openFile(path));
    return Ok(File{fd, path});
}

} // namespace Karm::Sys
