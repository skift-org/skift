#pragma once

#include <karm-ui/focus.h>
#include <karm-ui/layout.h>
#include <karm-ui/view.h>
#include <mdi/target.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_FOCUS{
    Mdi::TARGET,
    "Focusable",
    "A control that can be focused by the user.",
    [] {
        return Ui::vflow(
                   8,
                   Ui::labelLarge("Apple") | Ui::focusable(),
                   Ui::labelLarge("Banana") | Ui::focusable(),
                   Ui::labelLarge("Cherry") | Ui::focusable()
               ) |
               Ui::center();
    },
};

} // namespace Hideo::Zoo
