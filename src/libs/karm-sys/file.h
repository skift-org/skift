#pragma once

#include <karm-base/rc.h>
#include <karm-io/traits.h>
#include <karm-meta/utils.h>

#include "fd.h"
#include "path.h"

namespace Karm::Sys {

struct File :
    public Io::Reader,
    public Io::Writer,
    public Io::Seeker,
    public Io::Flusher,
    Meta::NoCopy {

    Strong<Fd> _fd;
    Path _path;

    File(Strong<Fd> fd, Path path) : _fd(fd), _path(path) {}

    static Result<File> create(Path path);

    static Result<File> open(Path path);

    Result<size_t> read(MutBytes bytes) override {
        return _fd->read(bytes);
    }

    Result<size_t> write(Bytes bytes) override {
        return _fd->write(bytes);
    }

    Result<size_t> seek(Io::Seek seek) override {
        return _fd->seek(seek);
    }

    Result<size_t> flush() override {
        return _fd->flush();
    }

    Strong<Fd> asFd() {
        return _fd;
    }
};

} // namespace Karm::Sys
