#pragma once

#include <karm-base/macros.h>
#include <karm-base/result.h>
#include <karm-io/types.h>
#include <karm-meta/same.h>
#include <karm-text/str.h>

#include "path.h"

namespace Karm::Embed {

using _Fd = int;

struct _Stat {
};

struct _Pipe {
    _Fd in;
    _Fd out;
};

auto _open(Sys::Path path) -> Base::Result<_Fd>;

auto _create(Sys::Path path) -> Base::Result<_Fd>;

auto _pipe() -> Base::Result<_Pipe>;

void _ref(_Fd fd);

void _deref(_Fd fd);

auto _read(_Fd fd, void *buf, size_t size) -> Base::Result<size_t>;

auto _write(_Fd fd, void const *buf, size_t size) -> Base::Result<size_t>;

auto _seek(_Fd fd, Io::Seek seek) -> Base::Result<size_t>;

auto _flush(_Fd fd) -> Base::Result<size_t>;

auto _dup(_Fd fd) -> Base::Result<_Fd>;

} // namespace Karm::Embed

namespace Karm::Sys {

struct Fd {
    Embed::_Fd _fd = -1;

    Fd(Embed::_Fd fd) : _fd(fd) {}

    Fd(Fd const &other) : _fd(other._fd) {
        Embed::_ref(_fd);
    }

    Fd(Fd &&other) : _fd(other._fd) { other._fd = -1; }

    ~Fd() {
        if (_fd != -1) {
            Embed::_deref(_fd);
        }
    }

    Fd &operator=(Fd const &other) {
        if (_fd != -1) {
            Embed::_deref(_fd);
        }

        _fd = other._fd;
        Embed::_ref(_fd);
        return *this;
    }

    Fd &operator=(Fd &&other) {
        if (_fd != -1) {
            Embed::_deref(_fd);
        }
        _fd = other._fd;
        other._fd = -1;
        return *this;
    }

    static Base::Result<Fd> open(Sys::Path path) {
        return Fd{try$(Embed::_open(path))};
    }

    static Base::Result<Fd> create(Sys::Path path) {
        return Fd{try$(Embed::_create(path))};
    }

    Base::Result<size_t> read(void *buf, size_t size) {
        return Embed::_read(_fd, buf, size);
    }

    Base::Result<size_t> write(void const *buf, size_t size) {
        return Embed::_write(_fd, buf, size);
    }

    Base::Result<size_t> seek(Io::Seek seek) {
        return Embed::_seek(_fd, seek);
    }

    Base::Result<size_t> flush() {
        return Embed::_flush(_fd);
    }

    Base::Result<Fd> dup() {
        return Fd{try$(Embed::_dup(_fd))};
    }
};

template <typename T>
concept AsFd = requires(T t) {
    { t.fd() } -> Meta::Same<Fd &>;
};

} // namespace Karm::Sys
