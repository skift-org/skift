#pragma once

import Karm.Core;

#include "defs.h"
#include "fd.h"

namespace Karm::Sys {

struct In : Io::Reader {
    Rc<Fd> _fd;

    In(Rc<Fd> fd)
        : _fd(fd) {}

    Res<usize> read(MutBytes bytes) override {
        return _fd->read(bytes);
    }

    Rc<Fd> fd() {
        return _fd;
    }
};

struct Out : Io::TextEncoderBase<Sys::Encoding> {
    Rc<Fd> _fd;

    Out(Rc<Fd> fd)
        : _fd(fd) {}

    Res<usize> write(Bytes bytes) override {
        return _fd->write(bytes);
    }

    Rc<Fd> fd() {
        return _fd;
    }

    Res<> flush() override {
        return _fd->flush();
    }
};

struct Err : Io::TextEncoderBase<Sys::Encoding> {
    Rc<Fd> _fd;

    Err(Rc<Fd> fd)
        : _fd(fd) {}

    Res<usize> write(Bytes bytes) override {
        return _fd->write(bytes);
    }

    Rc<Fd> fd() {
        return _fd;
    }

    Res<> flush() override {
        return _fd->flush();
    }
};

In& in();

Out& out();

Err& err();

inline void print(Str str = "", auto&&... args) {
    (void)Io::format(out(), str, std::forward<decltype(args)>(args)...);
}

inline void err(Str str = "", auto&&... args) {
    (void)Io::format(err(), str, std::forward<decltype(args)>(args)...);
}

inline void println(Str str = "", auto&&... args) {
    (void)Io::format(out(), str, std::forward<decltype(args)>(args)...);
    (void)out().writeStr(Str{Sys::LINE_ENDING});
    (void)out().flush();
}

inline void errln(Str str = "", auto&&... args) {
    (void)Io::format(err(), str, std::forward<decltype(args)>(args)...);
    (void)err().writeStr(Str{Sys::LINE_ENDING});
    (void)err().flush();
}

} // namespace Karm::Sys
