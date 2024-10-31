#pragma once

#include <karm-base/rc.h>

namespace Karm {

template <typename T>
struct Cow {
    Strong<T> _inner = base();

    static Opt<Strong<T>> _base;

    static Strong<T> base() {
        if (not _base) {
            _base = makeStrong<T>();
        }
        return _base.unwrap();
    }

    T &cow() {
        if (_inner.refs() > 1)
            _inner = makeStrong<T>(_inner.unwrap());
        return _inner.unwrap();
    }

    constexpr T const *operator->() const {
        return &_inner.unwrap();
    }

    constexpr T const &operator*() const {
        return _inner.unwrap();
    }

    bool sameInstance(Cow const &other) const {
        return &_inner == &other._inner;
    }
};

template <typename T>
Opt<Strong<T>> Cow<T>::_base = NONE;

template <typename T>
Cow<T> makeCow(T const &value) {
    return {makeStrong<T>(value)};
}

} // namespace Karm
