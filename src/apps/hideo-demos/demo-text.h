#pragma once

#include <karm-kira/row.h>
#include <karm-ui/layout.h>
#include <karm-ui/scroll.h>
#include <karm-ui/view.h>
#include <mdi/text.h>

#include "base.h"

namespace Hideo::Demos {

static inline Demo TEXT_DEMO{
    Mdi::TEXT,
    "Typography",
    "Typography",
    [] {
        return Ui::vflow(
                   Ui::displayLarge("Display Large"),
                   Ui::displayMedium("Display Medium"),
                   Ui::displaySmall("Display Small"),

                   Ui::headlineLarge("Headline Large"),
                   Ui::headlineMedium("Headline Medium"),
                   Ui::headlineSmall("Headline Small"),

                   Ui::titleLarge("Title Large"),
                   Ui::titleMedium("Title Medium"),
                   Ui::titleSmall("Title Small"),

                   Ui::labelLarge("Label Large"),
                   Ui::labelMedium("Label Medium"),
                   Ui::labelSmall("Label Small"),

                   Ui::bodyLarge("Body Large"),
                   Ui::bodyMedium("Body Medium"),
                   Ui::bodySmall("Body Small"),

                   Ui::codeLarge("Code Large"),
                   Ui::codeMedium("Code Medium"),
                   Ui::codeSmall("Code Small")
               ) |
               Ui::vscroll();
    },
};

} // namespace Hideo::Demos
