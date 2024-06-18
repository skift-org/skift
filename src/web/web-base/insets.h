#pragma once

#include "length.h"
#include "percent.h"

namespace Web {

struct MarginWidth {
    enum struct Type {
        AUTO,
        PERCENT,
        VALUE,
    };

    using enum Type;

    Type _type;
    union {
        Percent _percent;
        Length _value;
    };

    MarginWidth()
        : MarginWidth(Percent::ZERO) {
    }

    MarginWidth(Type type)
        : _type(type) {
    }

    MarginWidth(Percent percent)
        : _type(Type::PERCENT), _percent(percent) {
    }

    MarginWidth(Length value)
        : _type(Type::VALUE), _value(value) {
    }

    MarginWidth(PercentOr<Length> val) {
        if (val == PercentOr<Length>::PERCENT) {
            _type = Type::PERCENT;
            _percent = val._percent;
        } else {
            _type = Type::VALUE;
            _value = val._value;
        }
    }

    void repr(Io::Emit &e) const {
        if (_type == Type::AUTO) {
            e("auto");
        } else if (_type == Type::PERCENT) {
            e("{}", _percent);
        } else {
            e("{}", _value);
        }
    }

    bool operator==(Type type) const {
        return _type == type;
    }
};

struct Margin {
    MarginWidth top;
    MarginWidth right;
    MarginWidth bottom;
    MarginWidth left;
};

struct Padding {
    PercentOr<Length> top;
    PercentOr<Length> right;
    PercentOr<Length> bottom;
    PercentOr<Length> left;
};

} // namespace Web
