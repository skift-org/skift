#pragma once

#include <karm-base/distinct.h>
#include <karm-base/std.h>
#include <karm-io/emit.h>

namespace Web {

struct Percent : public Distinct<f64, struct _PercentTag> {
    using Distinct::Distinct;

    static Percent const ZERO;

    static Percent from(f64 val) {
        return Percent(val);
    }

    void repr(Io::Emit &e) const {
        e("{}%", value());
    }
};

inline Percent const Percent::ZERO = Percent(0);

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

    static PercentOr ZERO;

    PercentOr()
        : PercentOr(Percent::ZERO) {
    }

    PercentOr(Percent percent)
        : _type(Type::PERCENT), _percent(percent) {
    }

    PercentOr(T value)
        : _type(Type::VALUE), _value(value) {
    }

    void repr(Io::Emit &e) const {
        if (_type == Type::PERCENT) {
            e("{}", _percent);
        } else {
            e("{}", _value);
        }
    }

    bool operator==(Type type) const {
        return _type == type;
    }
};

template <typename T>
inline PercentOr<T> PercentOr<T>::ZERO = Percent::ZERO;

} // namespace Web
