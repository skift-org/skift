#pragma once

#include <karm-kira/color-input.h>
#include <karm-ui/layout.h>
#include <mdi/gradient-horizontal.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_HSV_SQUARE{
    Mdi::GRADIENT_HORIZONTAL,
    "HSV Square",
    "A control that allows the user to pick a color using a square.",
    [] {
        return Kr::hsvSquare({}, Ui::IGNORE<Gfx::Hsv>) |
               Ui::box({
                   .borderWidth = 1,
                   .borderFill = Ui::GRAY800,
               }) |
               Ui::center();
    },
};

} // namespace Hideo::Zoo
