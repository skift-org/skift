#pragma once

#include <karm-kira/number.h>
#include <karm-ui/layout.h>
#include <mdi/counter.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_NUMBER{
    Mdi::COUNTER,
    "Number",
    "An input where the user selects a value by incrementing or decrementing.",
    [] {
        return Kr::number(
                   100, [](auto &, ...) {
                   },
                   10
               ) |
               Ui::center();
    },
};

} // namespace Hideo::Zoo
