#pragma once

#include <hideo-base/row.h>
#include <karm-ui/layout.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>

#include "base.h"

namespace Hideo::Demos {

static inline Demo TEXT_DEMO{
    Mdi::TEXT,
    "Typography",
    "Typography",
    [] {
        return Ui::vflow(
                   Hideo::treeRow(
                       slot$(Ui::icon(Mdi::TEXT)), "Display", NONE,
                       slots$(
                           Ui::displayLarge("Display Large"),
                           Ui::displayMedium("Display Medium"),
                           Ui::displaySmall("Display Small")
                       )
                   ),

                   Hideo::treeRow(
                       slot$(Ui::icon(Mdi::TEXT)), "Headlines", NONE,
                       slots$(
                           Ui::headlineLarge("Headline Large"),
                           Ui::headlineMedium("Headline Medium"),
                           Ui::headlineSmall("Headline Small")
                       )
                   ),

                   Hideo::treeRow(
                       slot$(Ui::icon(Mdi::TEXT)), "Titles", NONE,
                       slots$(
                           Ui::titleLarge("Title Large"),
                           Ui::titleMedium("Title Medium"),
                           Ui::titleSmall("Title Small")
                       )
                   ),

                   Hideo::treeRow(
                       slot$(Ui::icon(Mdi::TEXT)), "Body", NONE,
                       slots$(
                           Ui::bodyLarge("Body Large"),
                           Ui::bodyMedium("Body Medium"),
                           Ui::bodySmall("Body Small")
                       )
                   )
               ) |
               Ui::vscroll();
    },
};

} // namespace Hideo::Demos
