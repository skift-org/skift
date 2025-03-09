#pragma once

#include <karm-ui/layout.h>
#include <mdi/clock.h>

#include "model.h"

import Karm.Kira;

namespace Hideo::Zoo {

static inline Page PAGE_CLOCK{
    Mdi::CLOCK,
    "Clock",
    "An analogue clock that display the time",
    [] {
        return Kr::clock({}) |
               Ui::pinSize({400, 300}) |
               Ui::center();
    },
};

} // namespace Hideo::Zoo
