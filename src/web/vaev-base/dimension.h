#pragma once

#include "angle.h"
#include "frequency.h"
#include "length.h"
#include "resolution.h"
#include "time.h"

namespace Vaev {

using _Dimension = Union<
    Length,
    Angle,
    Time,
    Frequency,
    Resolution>;

struct Dimension : _Dimension {
    using _Dimension::_Dimension;
};

} // namespace Vaev
