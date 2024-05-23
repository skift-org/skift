#pragma once

#include <karm-gfx/color.h>
#include <web-unit/angle.h>
#include <web-unit/display.h>
#include <web-unit/length.h>
#include <web-unit/resolution.h>
#include <web-unit/time.h>

namespace Web::CSSOM {

struct Initial {};
struct Inherit {};
struct Unset {};
struct Revert {};

using _Value = Union<
    Initial,
    Inherit,
    Unset,
    Revert,
    Unit::Angle,
    Gfx::Color,
    Unit::Display,
    Unit::Length,
    Unit::Resolution,
    Unit::Time>;

struct Value : public _Value {
    using _Value::_Value;
};

using _Calc = Union<
    Value>;

struct Calc : public _Calc {
    using _Calc::_Calc;
};

} // namespace Web::CSSOM
