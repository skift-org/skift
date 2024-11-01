#pragma once

#include <karm-kira/slider.h>
#include <karm-ui/layout.h>
#include <mdi/cat.h>
#include <mdi/tune-variant.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_SLIDER{
    Mdi::TUNE_VARIANT,
    "Slider",
    "An input where the user selects a value from within a given range.",
    [] {
        return Kr::slider(
                   0.5,
                   NONE,
                   Mdi::CAT,
                   "Cuteness"
               ) |
               Ui::minSize({320, Ui::UNCONSTRAINED}) |
               Ui::center();
    },
};

} // namespace Hideo::Zoo
