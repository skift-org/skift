#pragma once

#include <karm-kira/print-dialog.h>
#include <karm-ui/dialog.h>
#include <karm-ui/input.h>
#include <karm-ui/layout.h>
#include <mdi/printer.h>

#include "model.h"

namespace Hideo::Zoo {

static inline Page PAGE_PRINT_DIALOG{
    Mdi::PRINTER,
    "Print dialog",
    "Prompts the user to print a document.",
    [] {
        return Ui::button(
                   [](auto &n) {
                       Ui::showDialog(
                           n,
                           Kr::printDialog([](Print::Settings const &s) -> Vec<Print::Page> {
                               return {
                                   {s.paper},
                                   {s.paper},
                                   {s.paper},
                               };
                           })
                       );
                   },
                   "Show dialog"
               ) |
               Ui::center();
    },
};

} // namespace Hideo::Zoo
