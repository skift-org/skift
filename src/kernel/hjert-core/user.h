#pragma once

#include "objects.h"

namespace Hjert::Core {

template <typename T>
struct User {
    usize _addr;

    User(Hj::Arg addr)
        : _addr(addr) {
    }

    Hal::VmmRange vrange() {
        return Hal::VmmRange{_addr, sizeof(T)};
    }

    Res<T> load(Space &space) {
        if (_addr == 0) {
            return Error::invalidInput("null pointer");
        }

        ObjectLockScope scope(space);
        try$(space._validate(vrange()));
        return Ok(*reinterpret_cast<T *>(_addr));
    }

    Res<> store(Space &space, T const &val) {
        if (_addr == 0) {
            return Error::invalidInput("null pointer");
        }

        ObjectLockScope scope(space);
        try$(space._validate(vrange()));
        *reinterpret_cast<T *>(_addr) = val;
        return Ok();
    }
};

template <typename T>
struct UserSlice {
    usize _addr;
    usize _len;

    UserSlice(Hj::Arg addr, usize len)
        : _addr(addr),
          _len(len) {
    }

    Hal::VmmRange vrange() const {
        return Hal::VmmRange{_addr, sizeof(T) * _len};
    }

    usize len() const {
        return _len;
    }

    template <typename R>
    Res<> with(Space &space, auto f) {
        if (_addr == 0) {
            return Error::invalidInput("null pointer");
        }

        ObjectLockScope scope(space);
        try$(space._validate(vrange()));
        return f(R{reinterpret_cast<T *>(_addr), _len});
    }
};

} // namespace Hjert::Core
