module;

#include <karm-base/opt.h>
#include <karm-gfx/icon.h>
#include <karm-math/align.h>

export module Karm.Kira:navbar;

import Karm.Ui;
import :separator;

namespace Karm::Kira {

export Ui::Child navbarContent(Ui::Children children) {
    return Ui::vflow(
        separator(),
        Ui::hflow(
            4,
            children
        ) |
            Ui::box({
                .padding = 8,
                .backgroundFill = Ui::GRAY900,
            })
    );
}

export Ui::Child navbarItem(Opt<Ui::Send<>> onPress, Gfx::Icon icon, Str text, bool selected) {
    return Ui::button(
               std::move(onPress),
               selected
                   ? Ui::ButtonStyle::regular().withForegroundFill(Ui::ACCENT500)
                   : Ui::ButtonStyle::subtle(),
               Ui::vflow(
                   0,
                   Math::Align::CENTER,
                   Ui::icon(icon),
                   Ui::empty(4),
                   Ui::labelSmall(text)
               ) |
                   Ui::insets({10, 8, 6, 8})
           ) |
           Ui::grow();
}

} // namespace Karm::Kira
