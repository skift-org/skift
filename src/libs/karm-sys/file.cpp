#include <embed-sys/sys.h>

#include "file.h"

namespace Karm::Sys {

Res<File> File::create(Url url) {
    return Ok(File{try$(Embed::createFile(url)), url});
}

Res<File> File::open(Url url) {
    auto fd = try$(Embed::openFile(url));
    return Ok(File{fd, url});
}

} // namespace Karm::Sys
