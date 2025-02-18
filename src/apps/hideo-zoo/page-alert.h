#pragma once

#include <karm-kira/dialog.h>
#include <karm-ui/dialog.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <mdi/alert.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_ALERT{
    Mdi::ALERT,
    "Alert",
    "A modal dialog that interrupts the user with important content and expects a response.",
    [] {
        return Ui::button(
                   [](auto& n) {
                       Ui::showDialog(
                           n,
                           Kr::dialogContent({
                               Kr::dialogHeader({
                                   Kr::dialogTitle("Are you absolutely sure?"s),
                                   Kr::dialogDescription("This action cannot be undone. This will permanently delete your account and remove your data from our servers."s),
                               }),
                               Kr::dialogFooter({
                                   Kr::dialogCancel(),
                                   Kr::dialogAction(Ui::NOP, "Continue"s),
                               }),
                           })
                       );
                   },
                   "Show alert"
               ) |
               Ui::center();
    },
};

} // namespace Hideo::Zoo
