#pragma once

#include "button.h"

namespace Karm::Ui {

static inline Child row(Opt<Child> leading, String title, Opt<String> subtitle, Opt<Child> trailing) {
    auto t = subtitle
                 ? vflow(
                       8,
                       text(16, title),
                       text(*subtitle))
                 : text(16, title);

    return spacing(
        8,
        hflow(
            16,
            Layout::Align::VCENTER | Layout::Align::HFILL,

            spacing({0, 8, 0, 8}, leading ? *leading : empty()),
            grow(t),
            spacing({0, 8, 0, 8}, trailing ? *trailing : empty())));
}

} // namespace Karm::Ui
