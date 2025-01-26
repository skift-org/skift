#pragma once

#include <karm-base/union.h>

#include "length.h"
#include "numbers.h"
#include "percent.h"

namespace Vaev {

struct LineHeight {
    struct _Normal {};

    static constexpr _Normal NORMAL = {};
    Union<PercentOr<Length>, Number> _value;

    LineHeight(_Normal) : _value(1.2) {}

    LineHeight(PercentOr<Length> value) : _value(value) {}

    LineHeight(Number value) : _value(value) {}

    void repr(Io::Emit& e) const {
        e("{}", _value);
    }
};

} // namespace Vaev
