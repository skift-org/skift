#pragma once

#include "_prelude.h"

#include "error.h"
#include "inert.h"
#include "keywords.h"
#include "panic.h"
#include "std.h"
#include "try.h"

namespace Karm {

template <typename T>
struct Opt {
    bool _present{};
    Inert<T> _value{};

    Opt() {}

    Opt(None) {}

    Opt(T const &value) : _present(true) {
        _value.ctor(value);
    }

    Opt(T &&value) : _present(true) {
        _value.ctor(std::move(value));
    }

    Opt(Opt const &other) : _present(other._present) {
        if (_present) {
            _value.ctor(other.unwrap());
        }
    }

    Opt(Opt &&other) : _present(other._present) {
        if (_present) {
            _value.ctor(other.take());
        }
    }

    ~Opt() {
        release();
    }

    Opt &operator=(None) {
        release();
        return *this;
    }

    Opt &operator=(T const &value) {
        release();
        _present = true;
        _value.ctor(value);
        return *this;
    }

    Opt &operator=(T &&value) {
        release();
        _present = true;
        _value.ctor(std::move(value));
        return *this;
    }

    Opt &operator=(Opt const &other) {
        return *this = Opt(other);
    }

    Opt &operator=(Opt &&other) {
        release();
        if (other._present) {
            _present = true;
            _value.ctor(other.take());
        }
        return *this;
    }

    bool operator==(None) const {
        return !_present;
    }

    constexpr explicit operator bool() const {
        return _present;
    }

    T *operator->() {
        if (!_present) {
            panic("Unwrapping None");
        }
        return &_value.unwrap();
    }

    T &operator*() {
        if (!_present) {
            panic("Unwrapping None");
        }
        return _value.unwrap();
    }

    T const *operator->() const {
        if (!_present) {
            panic("Unwrapping None");
        }
        return &_value.unwrap();
    }

    T const &operator*() const {
        if (!_present) {
            panic("Unwrapping None");
        }
        return _value.unwrap();
    }

    void release() {
        if (_present) {
            _value.dtor();
            _present = false;
        }
    }

    None none() const {
        return NONE;
    }

    T &unwrap(const char *msg = "unwraping none") {
        if (!_present) {
            panic(msg);
        }
        return _value.unwrap();
    }

    T const &unwrap(const char *msg = "unwraping none") const {
        if (!_present) {
            panic(msg);
        }
        return _value.unwrap();
    }

    T take() {
        if (!_present) {
            panic("taking from none");
        }
        T v = _value.take();
        _present = false;
        return v;
    }

    bool with(auto visitor) {
        if (_present) {
            visitor(_value.unwrap());
            return true;
        }
        return false;
    }

    auto visit(auto visitor) -> decltype(visitor(_value.unwrap())) {
        if (_present) {
            return visitor(_value.unwrap());
        }

        return {};
    }
};

static_assert(Tryable<Opt<int>>);

} // namespace Karm
