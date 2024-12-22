#pragma once

#include <karm-io/fmt.h>
#include <karm-io/traits.h>
#include <karm-sys/_embed.h>

#include "fd.h"

namespace Karm::Sys {

struct In : public Io::Reader {
    Strong<Fd> _fd;

    In(Strong<Fd> fd)
        : _fd(fd) {}

    Res<usize> read(MutBytes bytes) override {
        return _fd->read(bytes);
    }

    Strong<Fd> fd() {
        return _fd;
    }
};

struct Out : public Io::TextWriterBase<> {
    Strong<Fd> _fd;

    Out(Strong<Fd> fd)
        : _fd(fd) {}

    Res<usize> write(Bytes bytes) override {
        return _fd->write(bytes);
    }

    Strong<Fd> fd() {
        return _fd;
    }

    Res<usize> flush() override {
        return _fd->flush();
    }
};

struct Err : public Io::TextWriterBase<> {
    Strong<Fd> _fd;

    Err(Strong<Fd> fd)
        : _fd(fd) {}

    Res<usize> write(Bytes bytes) override {
        return _fd->write(bytes);
    }

    Strong<Fd> fd() {
        return _fd;
    }

    Res<usize> flush() override {
        return _fd->flush();
    }
};

In &in();

Out &out();

Err &err();

inline void print(Str str = "", auto &&...args) {
    (void)Io::format(out(), str, std::forward<decltype(args)>(args)...);
}

inline void err(Str str = "", auto &&...args) {
    (void)Io::format(err(), str, std::forward<decltype(args)>(args)...);
}

inline void println(Str str = "", auto &&...args) {
    (void)Io::format(out(), str, std::forward<decltype(args)>(args)...);
    (void)out().writeStr(Str{Sys::LINE_ENDING});
    (void)out().flush();
}

inline void errln(Str str = "", auto &&...args) {
    (void)Io::format(err(), str, std::forward<decltype(args)>(args)...);
    (void)err().writeStr(Str{Sys::LINE_ENDING});
    (void)err().flush();
}

} // namespace Karm::Sys
