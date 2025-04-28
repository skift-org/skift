module;

#include <karm-app/inputs.h>
#include <karm-base/string.h>
#include <karm-math/align.h>

export module Karm.Kira:searchbar;

import Karm.Ui;
import Mdi;

namespace Karm::Kira {

export Ui::Child searchbar(String text) {
    return Ui::hflow(
               8,
               Math::Align::VCENTER | Math::Align::START,
               Ui::stack(
                   text ? Ui::empty() : Ui::labelMedium(Ui::GRAY600, "Search…"),
                   Ui::input(Ui::TextStyles::labelMedium(), text, Ui::SINK<String>)
               ) | Ui::grow(),
               Ui::icon(Mdi::MAGNIFY)
           ) |
           Ui::box({
               .padding = {6, 12, 6, 12},
               .borderRadii = 4,
               .borderWidth = 1,
               .borderFill = Ui::GRAY800,
           }) |
           Ui::minSize({Ui::UNCONSTRAINED, 36}) |
           Ui::focusable() |
           Ui::keyboardShortcut(App::Key::F, App::KeyMod::CTRL);
}

} // namespace Karm::Kira
