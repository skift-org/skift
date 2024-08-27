#pragma once

#include <karm-kira/row.h>
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
                   Kr::treeRow(
                       slot$(Ui::icon(Mdi::TEXT)), "Display"s, NONE,
                       slots$(
                           Ui::displayLarge("Display Large"),
                           Ui::displayMedium("Display Medium"),
                           Ui::displaySmall("Display Small")
                       )
                   ),

                   Kr::treeRow(
                       slot$(Ui::icon(Mdi::TEXT)), "Headlines"s, NONE,
                       slots$(
                           Ui::headlineLarge("Headline Large"),
                           Ui::headlineMedium("Headline Medium"),
                           Ui::headlineSmall("Headline Small")
                       )
                   ),

                   Kr::treeRow(
                       slot$(Ui::icon(Mdi::TEXT)), "Titles"s, NONE,
                       slots$(
                           Ui::titleLarge("Title Large"),
                           Ui::titleMedium("Title Medium"),
                           Ui::titleSmall("Title Small")
                       )
                   ),

                   Kr::treeRow(
                       slot$(Ui::icon(Mdi::TEXT)), "Body"s, NONE,
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
