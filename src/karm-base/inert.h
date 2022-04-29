#pragma once

#include "_prelude.h"

#include "std.h"

namespace Karm::Base {

template <typename T>
struct Inert {
    union {
        T _value;
    };

    Inert(){};

    ~Inert() {}

    template <typename... Args>
    void ctor(Args... args) {
        new (&_value) T(std::forward<Args>(args)...);
    }

    void dtor() {
        _value.~T();
    }

    auto unwrap() -> T & {
        return _value;
    }

    auto unwrap() const -> T const & {
        return _value;
    }

    auto take() -> T {
        T value = std::move(_value);
        dtor();
        return value;
    }
};

}; // namespace Karm::Base
