#pragma once

#include "space.h"

namespace Hjert::Core {

// A wrapper an userspace provided address.
// This is used to load and store values from userspace safely.
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
        ObjectLockScope scope(space);
        auto &v = *try$(_acquire(space));
        return Ok(v);
    }

    Res<> store(Space &space, T const &val) {
        ObjectLockScope scope(space);
        auto &v = *try$(_acquire(space));
        v = val;
        return Ok();
    }

    Res<T *> _acquire(Space &space) {
        if (_addr == 0)
            return Error::invalidInput("null pointer");
        try$(space._validate(vrange()));
        return Ok(reinterpret_cast<T *>(_addr));
    }
};

// A wrapper an userspace provided array.
// This is used to load and store values from userspace safely.
template <typename Slice>
struct UserSlice {
    using Inner = typename Slice::Inner;

    usize _addr;
    usize _len;

    UserSlice(usize addr, usize len)
        : _addr(addr),
          _len(len) {
    }

    Hal::VmmRange vrange() const {
        return Hal::VmmRange{_addr, sizeof(Inner) * _len};
    }

    usize len() const {
        return _len;
    }

    Res<> with(Space &space, auto f) {
        ObjectLockScope scope(space);
        return f(try$(_acquire(space)));
    }

    Res<Slice> _acquire(Space &space) {
        if (_len == 0uz)
            return Ok(Slice{nullptr, 0uz});

        if (_addr == 0)
            return Error::invalidInput("null pointer");

        try$(space._validate(vrange()));
        return Ok(Slice{reinterpret_cast<Inner *>(_addr), _len});
    }
};

template <typename... Args>
static inline Res<> with(Space &space, auto f, Args &&...args) {
    ObjectLockScope scope(space);
    Tuple acquired{args._acquire(space)...};
    try$(acquired.visit([](auto &a) -> Res<> {
        try$(a);
        return Ok();
    }));
    return acquired.apply([&](auto &...args) -> Res<> {
        return f(args.unwrap()...);
    });
}

} // namespace Hjert::Core
