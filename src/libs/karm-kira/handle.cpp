module;

#include <karm-base/opt.h>

export module Karm.Kira:handle;

import Karm.Ui;

namespace Karm::Kira {

export Ui::Child handle() {
    return Ui::empty({128, 4}) |
           Ui::box({
               .borderRadii = 999,
               .backgroundFill = Ui::GRAY50,
           }) |
           Ui::insets(12) |
           Ui::center() |
           Ui::minSize({Ui::UNCONSTRAINED, 48});
}

export Ui::Child dragHandle() {
    return handle() | Ui::dragRegion();
}

export Ui::Child buttonHandle(Opt<Ui::Send<>> press) {
    return handle() |
           Ui::button(std::move(press), Ui::ButtonStyle::none());
}

} // namespace Karm::Kira
