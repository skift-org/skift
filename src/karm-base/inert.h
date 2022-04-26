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

    Inert(T &&other) {
        construct(std::forward<T>(other));
    }

    Inert &operator=(T &&other) {
        construct(std::forward<T>(other));
        return *this;
    }

    ~Inert() {}

    template <typename... Args>
    void construct(Args... args) {
        new (&_value) T(std::forward<Args>(args)...);
    }

    void destroy() {
        _value.~T();
    }

    T move() {
        T value = std::move(_value);
        destroy();
        return value;
    }

    T &unwrap() { return _value; }
};

}; // namespace Karm::Base
