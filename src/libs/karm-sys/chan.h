#pragma once

#include <embed-sys/sys.h>
#include <karm-fmt/fmt.h>
#include <karm-io/traits.h>

#include "fd.h"

namespace Karm::Sys {

struct In : public Io::Reader {
    Strong<Fd> _fd;

    In(Strong<Fd> fd)
        : _fd(fd) {}

    Res<usize> read(MutBytes bytes) override {
        return _fd->read(bytes);
    }
};

struct Out : public Io::TextWriterBase<> {
    Strong<Fd> _fd;

    Out(Strong<Fd> fd)
        : _fd(fd) {}

    Res<usize> write(Bytes bytes) override {
        return _fd->write(bytes);
    }
};

struct Err : public Io::TextWriterBase<> {
    Strong<Fd> _fd;

    Err(Strong<Fd> fd)
        : _fd(fd) {}

    Res<usize> write(Bytes bytes) override {
        return _fd->write(bytes);
    }
};

In &in();

Out &out();

Err &err();

inline void print(Str str, auto &&...args) {
    (void)Fmt::format(out(), str, std::forward<decltype(args)>(args)...);
}

inline void err(Str str, auto &&...args) {
    (void)Fmt::format(err(), str, std::forward<decltype(args)>(args)...);
}

inline void println(Str str, auto &&...args) {
    (void)Fmt::format(out(), str, std::forward<decltype(args)>(args)...);
    (void)out().writeStr(Embed::LINE_ENDING);
}

inline void errln(Str str, auto &&...args) {
    (void)Fmt::format(err(), str, std::forward<decltype(args)>(args)...);
    (void)out().writeStr(Embed::LINE_ENDING);
}

} // namespace Karm::Sys
