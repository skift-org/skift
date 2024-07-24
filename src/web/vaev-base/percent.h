#pragma once

#include <karm-base/distinct.h>
#include <karm-io/emit.h>

#include "base.h"

namespace Vaev {

using Percent = Distinct<f64, struct _PercentTag>;

template <typename T>
struct PercentOf {
    f64 _val;

    void repr(Io::Emit &e) const {
        e("{}%", _val);
    }
};

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
        return _type == Type::VALUE and _value.resolved();
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

template <typename T>
struct ValueContext<PercentOr<T>> : public ValueContext<T> {
    using Resolved = ValueContext<T>::Resolved;

    T percentRelative;

    Resolved resolve(PercentOr<T> value) {
        if (value == PercentOr<T>::Type::PERCENT) {
            return {percentRelative * (value.percent().value() / 100.)};
        }
        return ValueContext<T>::resolve(value.value());
    }
};

} // namespace Vaev

template <>
struct Karm::Io::Repr<Vaev::Percent> {
    static void repr(Io::Emit &e, Vaev::Percent const &v) {
        e("{}%", v.value());
    }
};
