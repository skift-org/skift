#pragma once

#include <karm-base/distinct.h>
#include <karm-io/emit.h>

namespace Vaev {

using Percent = Distinct<f64, struct _PercentTag>;

template <typename T>
struct PercentOr {
    enum Type : u8 {
        PERCENT,
        VALUE,
    };

    using Resolved = typename T::Resolved;

    Type _type;

    union {
        Percent _percent;
        T _value;
    };

    constexpr PercentOr()
        : PercentOr(T{}) {
    }

    constexpr PercentOr(Percent percent)
        : _type(Type::PERCENT), _percent(percent) {
    }

    constexpr PercentOr(T value)
        : _type(Type::VALUE), _value(value) {
    }

    void repr(Io::Emit& e) const {
        if (_type == Type::PERCENT) {
            e("{}", _percent);
        } else {
            e("{}", _value);
        }
    }

    constexpr bool operator==(Type type) const {
        return _type == type;
    }

    constexpr bool operator==(PercentOr const& other) const {
        if (_type != other._type)
            return false;

        if (_type == Type::PERCENT)
            return _percent == other._percent;

        return _value == other._value;
    }

    constexpr bool resolved() const {
        return _type == Type::VALUE;
    }

    T value() const {
        if (_type == Type::PERCENT)
            panic("cannot get value of percent");
        return _value;
    }

    Percent percent() const {
        if (_type == Type::VALUE)
            panic("cannot get percent of value");
        return _percent;
    }
};

} // namespace Vaev

template <>
struct Karm::Io::Repr<Vaev::Percent> {
    static void repr(Io::Emit& e, Vaev::Percent const& v) {
        e("{}%", v.value());
    }
};
