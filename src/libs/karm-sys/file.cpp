#include "file.h"

#include "_embed.h"
#include "proc.h"

namespace Karm::Sys {

Res<FileWriter> File::create(Mime::Url url) {
    try$(ensureUnrestricted());
    auto fd = try$(_Embed::createFile(url));
    return Ok(FileWriter{fd, url});
}

Res<FileReader> File::open(Mime::Url url) {
    try$(ensureUnrestricted());
    auto fd = try$(_Embed::openFile(url));
    return Ok(FileReader{fd, url});
}

Res<File> File::openOrCreate(Mime::Url url) {
    try$(ensureUnrestricted());
    auto fd = try$(_Embed::openOrCreateFile(url));
    return Ok(File{fd, url});
}

} // namespace Karm::Sys
