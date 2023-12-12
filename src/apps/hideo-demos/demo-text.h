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
    []() {
        return Ui::vflow(
                   Hideo::treeRow(Ui::icon(Mdi::TEXT), "Display", NONE,
                                  {
                                      Ui::displayLarge("Display Large"),
                                      Ui::displayMedium("Display Medium"),
                                      Ui::displaySmall("Display Small"),
                                  }),

                   Hideo::treeRow(Ui::icon(Mdi::TEXT), "Headlines", NONE,
                                  {
                                      Ui::headlineLarge("Headline Large"),
                                      Ui::headlineMedium("Headline Medium"),
                                      Ui::headlineSmall("Headline Small"),
                                  }),

                   Hideo::treeRow(Ui::icon(Mdi::TEXT), "Titles", NONE,
                                  {
                                      Ui::titleLarge("Title Large"),
                                      Ui::titleMedium("Title Medium"),
                                      Ui::titleSmall("Title Small"),
                                  }),

                   Hideo::treeRow(Ui::icon(Mdi::TEXT), "Body", NONE,
                                  {
                                      Ui::bodyLarge("Body Large"),
                                      Ui::bodyMedium("Body Medium"),
                                      Ui::bodySmall("Body Small"),
                                  })) |
               Ui::vscroll();
    },
};

} // namespace Hideo::Demos
