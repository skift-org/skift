#pragma once

#include "button.h"
#include "layout.h"
#include "view.h"

namespace Karm::Ui {

inline Child row(Opt<Child> leading, String title, Opt<String> subtitle, Opt<Child> trailing) {
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

inline Child buttonRow(OnPress onPress, Media::Icons i, String title, String subtitle) {
    return spacing(
        {8, 0},
        button(
            std::move(onPress),
            ButtonStyle::subtle(),
            row(
                icon(i, 24),
                title,
                subtitle,
                NONE)));
}

} // namespace Karm::Ui
