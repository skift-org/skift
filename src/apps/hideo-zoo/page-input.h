#pragma once

#include <karm-kira/input.h>
#include <karm-ui/layout.h>
#include <mdi/form-textbox.h>

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
                   Kr::input(Mdi::FORM_TEXTBOX, "Username"s, ""s) | Ui::pinSize({240, Ui::UNCONSTRAINED}),
                   Kr::input(Mdi::FORM_TEXTBOX, "Email"s, ""s) | Ui::pinSize({240, Ui::UNCONSTRAINED}),
                   Kr::input(Mdi::FORM_TEXTBOX, "Password"s, ""s) | Ui::pinSize({240, Ui::UNCONSTRAINED})
               ) |
               Ui::center();
    },
};

} // namespace Hideo::Zoo
