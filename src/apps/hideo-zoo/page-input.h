#pragma once

#include <karm-kira/input.h>
#include <karm-ui/layout.h>
#include <mdi/account.h>
#include <mdi/email.h>
#include <mdi/form-textbox.h>
#include <mdi/lock.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_INPUT{
    Mdi::FORM_TEXTBOX,
    "Input",
    "Displays a form input field or a component that looks like an input field.",
    [] {
        return Ui::vflow(
                   16,
                   Math::Align::CENTER,
                   Kr::input(Mdi::ACCOUNT, "Username"s, ""s) | Ui::pinSize({240, Ui::UNCONSTRAINED}),
                   Kr::input(Mdi::EMAIL, "Email"s, ""s) | Ui::pinSize({240, Ui::UNCONSTRAINED}),
                   Kr::input(Mdi::LOCK, "Password"s, ""s) | Ui::pinSize({240, Ui::UNCONSTRAINED}),
                   Kr::input("Text"s, ""s) | Ui::pinSize({240, Ui::UNCONSTRAINED})
               ) |
               Ui::center();
    },
};

} // namespace Hideo::Zoo
