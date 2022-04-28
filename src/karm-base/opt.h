#pragma once

#include <karm-debug/logger.h>
#include <karm-debug/panic.h>

#include "_prelude.h"

#include "inert.h"
#include "macros.h"
#include "std.h"

namespace Karm::Base {

struct None {
};

static constexpr None NONE = None{};

template <typename T>
struct Opt {
    bool _present = false;

    Inert<T> _value;

    Opt(){};

    Opt(None){};

    Opt(T const &value) : _present(true) {
        _value.ctor(value);
    }

    Opt(T &&value) : _present(true) {
        _value.ctor(std::forward<T>(value));
    }

    Opt(Opt const &other) : _present(other._present) {
        if (_present) {
            _value.ctor(other._value);
        }
    }

    Opt(Opt &&other) : _present(other._present) {
        if (_present) {
            _value.ctor(other._value.take());
            other.release();
        }
    }

    ~Opt() { release(); }

    auto operator=(None) -> Opt & {
        release();
        return *this;
    }

    auto operator=(T const &value) -> Opt & {
        release();
        _present = true;
        _value.ctor(value);
        return *this;
    }

    auto operator=(T &&value) -> Opt & {
        release();
        _present = true;
        _value.ctor(std::forward<T>(value));
        return *this;
    }

    auto operator=(Opt const &other) -> Opt & {
        return *this = Opt(other);
    }

    auto operator=(Opt &&other) -> Opt & {
        release();
        if (other._present) {
            _value.ctor(other._value.take());
            other.release();
        }
        return *this;
    }

    operator bool() const { return _present; }

    auto operator->() -> T * { return &_value; }

    auto operator*() -> T & { return *_value; }

    auto operator->() const -> T const * { return &_value; }

    auto operator*() const -> T const & { return *_value; }

    void release() {
        if (_present) {
            _value.dtor();
            _present = false;
        }
    }

    auto none() -> bool {
        return _present;
    }

    auto unwrap() -> T & {
        if (!_present) {
            Debug::panic("Unwrapping None");
        }
        return _value.unwrap();
    }

    auto take() -> T {
        if (!_present) {
            Debug::panic("Taking None");
        }
        T v = _value.take();
        release();
        return v;
    }

    void with(auto visitor) {
        if (_present) {
            visitor(_value.unwrap());
        }
    }

    void visit(auto visitor) {
        if (_present) {
            visitor(_value.unwrap());
        }
    }
};

static_assert(Tryable<Karm::Base::Opt<int>>);

} // namespace Karm::Base
