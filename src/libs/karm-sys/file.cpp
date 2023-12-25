#include "file.h"

#include "_embed.h"

namespace Karm::Sys {

Res<FileWriter> File::create(Url::Url url) {
    auto fd = try$(_Embed::createFile(url));
    return Ok(FileWriter{fd, url});
}

Res<FileReader> File::open(Url::Url url) {
    auto fd = try$(_Embed::openFile(url));
    return Ok(FileReader{fd, url});
}

Res<File> File::openOrCreate(Url::Url url) {
    auto fd = try$(_Embed::openOrCreateFile(url));
    return Ok(File{fd, url});
}

} // namespace Karm::Sys
