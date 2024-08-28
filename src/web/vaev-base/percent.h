#pragma once

#include <karm-base/distinct.h>
#include <karm-io/emit.h>

namespace Vaev {

using Percent = Distinct<f64, struct _PercentTag>;

template <typename T>
struct PercentOr {
    enum struct Type {
        PERCENT,
        VALUE,
    };

    using enum Type;

    Type _type;

    union {
        Percent _percent;
        T _value;
    };

    constexpr PercentOr()
        : PercentOr(Percent{}) {
    }

    constexpr PercentOr(Percent percent)
        : _type(Type::PERCENT), _percent(percent) {
    }

    constexpr PercentOr(T value)
        : _type(Type::VALUE), _value(value) {
    }

    void repr(Io::Emit &e) const {
        if (_type == Type::PERCENT) {
            e("{}", _percent);
        } else {
            e("{}", _value);
        }
    }

    constexpr bool operator==(Type type) const {
        return _type == type;
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
    static void repr(Io::Emit &e, Vaev::Percent const &v) {
        e("{}%", v.value());
    }
};
