#include "file.h"

#include "_embed.h"
#include "proc.h"

namespace Karm::Sys {

Res<FileWriter> File::create(Ref::Url url) {
    try$(ensureUnrestricted());
    auto fd = try$(_Embed::createFile(url));
    return Ok(FileWriter{fd, url});
}

Res<FileReader> File::open(Ref::Url url) {
    if (url.scheme == "data") {
        auto fd = makeRc<BlobFd>(try$(url.blob));
        return Ok(FileReader{fd, url});
    }

    if (url.scheme != "bundle")
        try$(ensureUnrestricted());

    auto fd = try$(_Embed::openFile(url));
    return Ok(FileReader{fd, url});
}

Res<File> File::openOrCreate(Ref::Url url) {
    try$(ensureUnrestricted());
    auto fd = try$(_Embed::openOrCreateFile(url));
    return Ok(File{fd, url});
}

} // namespace Karm::Sys
