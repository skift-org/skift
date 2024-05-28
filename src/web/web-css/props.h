#pragma once

#include <web-unit/display.h>

#include "computed-values.h"

namespace Web::Css {

struct DisplayProps {
    using Type = Unit::Display;

    Unit::Display value;

    static Str name() { return "display"; }

    static Unit::Display initial() {
        return {Unit::Display::FLOW, Unit::Display::INLINE};
    }

    void apply(ComputedValues &computed) const {
        computed.display = value;
    }
};

} // namespace Web::Css
