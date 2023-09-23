#include "file.h"

#include "_embed.h"

namespace Karm::Sys {

Res<File> File::create(Url::Url url) {
    return Ok(File{try$(_Embed::createFile(url)), url});
}

Res<File> File::open(Url::Url url) {
    auto fd = try$(_Embed::openFile(url));
    return Ok(File{fd, url});
}

} // namespace Karm::Sys
