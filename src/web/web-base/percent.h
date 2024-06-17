#pragma once

#include <karm-base/distinct.h>
#include <karm-base/std.h>
#include <karm-io/emit.h>

namespace Web {

struct Percent : public Distinct<f64, struct _PercentTag> {
    using Distinct::Distinct;

    static Percent from(f64 val) {
        return Percent(val);
    }

    void repr(Io::Emit &e) const {
        e("{}%", value());
    }
};

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
        Percent percent;
        T value;
    };

    PercentOr(Percent percent)
        : _type(Type::PERCENT), percent(percent) {
    }

    PercentOr(T value)
        : _type(Type::VALUE), value(value) {
    }

    void repr(Io::Emit &e) const {
        if (_type == Type::PERCENT) {
            e("{}%", percent.value());
        } else {
            e("{}", value);
        }
    }
};

} // namespace Web
