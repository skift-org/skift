#pragma once

#include <karm-base/rc.h>

namespace Karm {

template <typename T>
struct Cow {
    static Strong<T> BASE;
    Strong<T> _inner = BASE;

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
Strong<T> Cow<T>::BASE = makeStrong<T>();

template <typename T>
Cow<T> makeCow(T const &value) {
    return {makeStrong<T>(value)};
}

} // namespace Karm
