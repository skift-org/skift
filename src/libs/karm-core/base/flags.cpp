export module Karm.Core:base.flags;

import :base.enum_;

namespace Karm {

export template <Meta::Enum E, typename U = Meta::UnderlyingType<E>>
struct Flags {
    U _value = 0;

    static Flags fromUnderlying(U underlying) {
        Flags f;
        f._value = underlying;
        return f;
    }

    Flags() = default;

    Flags(None) {};

    Flags(E value)
        : _value(toUnderlyingType(value)) {}

    Flags(std::initializer_list<E> values) {
        for (auto value : values)
            _value |= toUnderlyingType(value);
    }

    bool has(Flags other) const {
        return (_value & other._value) == other._value;
    }

    void set(Flags other) {
        _value |= other._value;
    }

    void set(E value, bool on = true) {
        if (on) {
            _value |= toUnderlyingType(value);
        } else {
            _value &= ~toUnderlyingType(value);
        }
    }

    void unset(Flags other) {
        _value &= ~other._value;
    }

    void toggle(E value) {
        _value ^= toUnderlyingType(value);
    }

    void clear() {
        _value = 0;
    }

    bool empty() const {
        return _value == 0;
    }

    bool any() const {
        return _value != 0;
    }

    U raw() const {
        return _value;
    }

    operator bool() const {
        return _value != 0;
    }

    Flags operator~() const {
        Flags res;
        res._value = ~_value;
        return res;
    }

    Flags operator|(Flags other) const {
        Flags res;
        res._value = _value | other._value;
        return res;
    }

    Flags operator&(Flags other) const {
        Flags res;
        res._value = _value & other._value;
        return res;
    }

    Flags operator^(Flags other) const {
        Flags res;
        res._value = _value ^ other._value;
        return res;
    }

    Flags& operator|=(Flags other) {
        _value |= other._value;
        return *this;
    }

    Flags& operator&=(Flags other) {
        _value &= other._value;
        return *this;
    }

    Flags& operator^=(Flags other) {
        _value ^= other._value;
        return *this;
    }

    bool operator!() const {
        return !_value;
    }

    bool operator==(Flags other) const {
        return _value == other._value;
    }

    bool operator!=(Flags other) const {
        return _value != other._value;
    }
};

export template <Meta::Enum E, Meta::Same<E>... Es>
Flags(E, Es...) -> Flags<E>;

} // namespace Karm
