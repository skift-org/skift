#pragma once

#include "calc.h"
#include "keywords.h"
#include "length.h"

namespace Vaev {

static constexpr Au THIN_VALUE = 1_au;
static constexpr Au MEDIUM_VALUE = 3_au;
static constexpr Au THICK_VALUE = 5_au;

using LineWidth = Union<
    Keywords::Thin,
    Keywords::Medium,
    Keywords::Thick,
    CalcValue<Length>>;

} // namespace Vaev
