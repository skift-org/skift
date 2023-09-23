#pragma once

#include <karm-base/rc.h>
#include <karm-io/traits.h>
#include <karm-meta/nocopy.h>

#include "fd.h"

namespace Karm::Sys {

struct File :
    public Io::Reader,
    public Io::Writer,
    public Io::Seeker,
    public Io::Flusher,
    Meta::NoCopy {

    Strong<Fd> _fd;
    Url::Url _url;

    File(Strong<Fd> fd, Url::Url url) : _fd(fd), _url(url) {}

    static Res<File> create(Url::Url url);

    static Res<File> open(Url::Url url);

    Res<usize> read(MutBytes bytes) override {
        return _fd->read(bytes);
    }

    Res<usize> write(Bytes bytes) override {
        return _fd->write(bytes);
    }

    Res<usize> seek(Io::Seek seek) override {
        return _fd->seek(seek);
    }

    Res<usize> flush() override {
        return _fd->flush();
    }

    Strong<Fd> asFd() {
        return _fd;
    }
};

} // namespace Karm::Sys
