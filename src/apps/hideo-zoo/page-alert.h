#pragma once

#include <karm-kira/alert.h>
#include <karm-ui/dialog.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_ALERT{
    Mdi::ALERT,
    "Alert",
    "A modal dialog that interrupts the user with important content and expects a response.",
    [] {
        return Ui::button(
                   [](auto &n) {
                       Ui::showDialog(
                           n,
                           Kr::alertContent({
                               Kr::alertHeader({
                                   Kr::alertTitle("Are you absolutely sure?"s),
                                   Kr::alertDescription("This action cannot be undone. This will permanently delete your account and remove your data from our servers."s),
                               }),
                               Kr::alertFooter({
                                   Ui::grow(NONE),
                                   Kr::alertCancel(),
                                   Kr::alertAction(Ui::NOP, "Continue"s),
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
