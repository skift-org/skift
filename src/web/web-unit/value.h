#pragma once

#include <karm-gfx/color.h>

#include "angle.h"
#include "display.h"
#include "length.h"
#include "resolution.h"
#include "time.h"

namespace Web::Unit {

using _Value = Union<
    Unit::Angle,
    Gfx::Color,
    Unit::Display,
    Unit::Length,
    Unit::Resolution,
    Unit::Time>;

struct Value : public _Value {
    using _Value::_Value;
};

} // namespace Web::Unit
