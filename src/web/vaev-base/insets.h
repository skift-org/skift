#pragma once

#include "length.h"
#include "percent.h"

namespace Vaev {

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

    constexpr MarginWidth()
        : MarginWidth(Percent{}) {
    }

    constexpr MarginWidth(Type type)
        : _type(type) {
    }

    constexpr MarginWidth(Percent percent)
        : _type(Type::PERCENT), _percent(percent) {
    }

    constexpr MarginWidth(Length value)
        : _type(Type::VALUE), _value(value) {
    }

    constexpr MarginWidth(PercentOr<Length> val) {
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

using Margin = Math::Spacing<MarginWidth>;

using Padding = Math::Spacing<PercentOr<Length>>;

} // namespace Vaev
