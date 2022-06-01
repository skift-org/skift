#pragma once

#include "keywords.h"
#include "std.h"

namespace Karm {

struct Trilean {
    enum : uint8_t {
        _FALSE,
        _NONE,
        _TRUE,
    } _value;

    constexpr Trilean() : _value(_NONE) {}
    constexpr Trilean(bool value) : _value(value ? _TRUE : _FALSE) {}
    constexpr Trilean(None) : _value(_NONE) {}

    constexpr bool isTrue() const { return _value == _TRUE; }
    constexpr bool isFalse() const { return _value == _FALSE; }
    constexpr bool isNone() const { return _value == _NONE; }

    constexpr operator bool() const { return isTrue(); }
    constexpr bool operator==(None) const { return _value == _NONE; }
    constexpr bool operator!=(None) const { return _value != _NONE; }
    constexpr bool operator==(bool value) const { return _value == (value ? _TRUE : _FALSE); }
    constexpr bool operator!=(bool value) const { return _value != (value ? _TRUE : _FALSE); }
    constexpr bool operator==(Trilean const &other) const { return _value == other._value; }
    constexpr bool operator!=(Trilean const &other) const { return _value != other._value; }

    constexpr Trilean operator!() const {
        if (_value == _TRUE) {
            return _FALSE;
        }

        if (_value == _FALSE) {
            return _TRUE;
        }

        return _NONE;
    }

    constexpr Trilean operator&&(Trilean const &other) const {
        if (_value == _FALSE || other._value == _FALSE) {
            return _FALSE;
        }

        if (_value == _TRUE && other._value == _TRUE) {
            return _TRUE;
        }

        return _NONE;
    }

    constexpr Trilean operator||(Trilean const &other) const {
        if (_value == _TRUE || other._value == _TRUE) {
            return _TRUE;
        }

        if (_value == _FALSE && other._value == _FALSE) {
            return _FALSE;
        }

        return _NONE;
    }
};

} // namespace Karm
