#pragma once

#include <karm-ui/layout.h>
#include <karm-ui/row.h>
#include <karm-ui/view.h>

#include "base.h"

namespace Demos {

static inline Demo TEXT_DEMO{
    Mdi::TEXT,
    "Typography",
    "Typography",
    []() {
        return Ui::vflow(
            Ui::treeRow(Ui::icon(Mdi::TEXT), "Display", NONE,
                        {
                            Ui::text(Ui::TextStyle::displayLarge(), "Display Large"),
                            Ui::text(Ui::TextStyle::displayMedium(), "Display Medium"),
                            Ui::text(Ui::TextStyle::displaySmall(), "Display Small"),
                        }),

            Ui::treeRow(Ui::icon(Mdi::TEXT), "Headlines", NONE,
                        {
                            Ui::text(Ui::TextStyle::headlineLarge(), "Headline Large"),
                            Ui::text(Ui::TextStyle::headlineMedium(), "Headline Medium"),
                            Ui::text(Ui::TextStyle::headlineSmall(), "Headline Small"),
                        }),

            Ui::treeRow(Ui::icon(Mdi::TEXT), "Titles", NONE,
                        {
                            Ui::text(Ui::TextStyle::titleLarge(), "Title Large"),
                            Ui::text(Ui::TextStyle::titleMedium(), "Title Medium"),
                            Ui::text(Ui::TextStyle::titleSmall(), "Title Small"),
                        }),

            Ui::treeRow(Ui::icon(Mdi::TEXT), "Body", NONE,
                        {
                            Ui::text(Ui::TextStyle::bodyLarge(), "Body Large"),
                            Ui::text(Ui::TextStyle::bodyMedium(), "Body Medium"),
                            Ui::text(Ui::TextStyle::bodySmall(), "Body Small"),
                        }));
    },
};

} // namespace Demos
