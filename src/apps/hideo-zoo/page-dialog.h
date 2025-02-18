#pragma once

#include <karm-kira/dialog.h>
#include <karm-ui/dialog.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <mdi/information.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_DIALOG{
    Mdi::INFORMATION,
    "Dialog",
    "A window overlaid on either the primary window or another dialog window, rendering the content underneath inert.",
    [] {
        return Ui::button(
                   [](auto& n) {
                       Ui::showDialog(
                           n,
                           Kr::dialogContent({
                               Kr::dialogTitleBar("Dialog"s),
                               Ui::labelLarge("Hello, world") | Ui::center() | Ui::pinSize({200, 160}),
                           })
                       );
                   },
                   "Show dialog"
               ) |
               Ui::center();
    },
};

} // namespace Hideo::Zoo
