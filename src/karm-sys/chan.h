#pragma once

#include <embed/sys.h>
#include <karm-fmt/fmt.h>
#include <karm-io/traits.h>

#include "fd.h"

namespace Karm::Sys {

struct In :
    public Io::Reader {
    Strong<Fd> _fd;

    In(Strong<Fd> fd) : _fd(fd) {}

    Result<size_t> read(MutBytes bytes) override {
        return _fd->read(bytes);
    }
};

struct Out :
    public Io::TextWriter<Embed::Encoding> {
    Strong<Fd> _fd;

    Out(Strong<Fd> fd) : _fd(fd) {}

    Result<size_t> write(Bytes bytes) override {
        return _fd->write(bytes);
    }
};

struct Err :
    public Io::TextWriter<Embed::Encoding> {
    Strong<Fd> _fd;

    Err(Strong<Fd> fd) : _fd(fd) {}

    Result<size_t> write(Bytes bytes) override {
        return _fd->write(bytes);
    }
};

In &in();

Out &out();

Err &err();

static inline void println(Str str, auto &&...args) {
    (void)Fmt::format(out(), str, std::forward<decltype(args)>(args)...);
    (void)out().writeRune(U'\n');
}

static inline void errln(Str str, auto &&...args) {
    (void)Fmt::format(err(), str, std::forward<decltype(args)>(args)...);
    (void)out().writeRune(U'\n');
}

} // namespace Karm::Sys
