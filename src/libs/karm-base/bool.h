#pragma once

#include <karm-meta/traits.h>

namespace Karm {

struct Bool;

struct Bool {
    bool _val;

    Bool() = delete;

    constexpr Bool(Meta::Boolean auto value)
        : _val(value) {}

    constexpr Bool &operator=(Meta::Boolean auto value) {
        _val = value;
        return *this;
    }

    explicit constexpr operator bool() const {
        return _val;
    }

    constexpr Bool operator!() const {
        return Bool(not _val);
    }
};

constexpr bool operator==(Bool a, Bool b) {
    return static_cast<bool>(a) == static_cast<bool>(b);
}

constexpr bool operator==(Bool a, Meta::Boolean auto b) {
    return static_cast<bool>(a) == static_cast<bool>(b);
}

constexpr bool operator==(Meta::Boolean auto a, Bool &b) {
    return static_cast<bool>(a) == static_cast<bool>(b);
}

constexpr bool operator!=(Bool a, Bool b) {
    return static_cast<bool>(a) != static_cast<bool>(b);
}

constexpr bool operator!=(Bool a, Meta::Boolean auto b) {
    return static_cast<bool>(a) != static_cast<bool>(b);
}

constexpr bool operator!=(Meta::Boolean auto a, Bool b) {
    return static_cast<bool>(a) != static_cast<bool>(b);
}

} // namespace Karm
