module;

#include <karm-base/string.h>
#include <karm-gfx/icon.h>

export module Karm.Kira:avatar;

import Karm.Ui;
import Mdi;

namespace Karm::Kira {

export Ui::Child avatar(String t) {
    Ui::BoxStyle boxStyle = {
        .borderRadii = 99,
        .backgroundFill = Ui::GRAY800,
        .foregroundFill = Ui::GRAY500
    };

    return Ui::labelLarge(t) |
           Ui::center() |
           Ui::pinSize(46) |
           Ui::box(boxStyle);
}

export Ui::Child avatar(Gfx::Icon i) {
    Ui::BoxStyle boxStyle = {
        .borderRadii = 99,
        .backgroundFill = Ui::GRAY800,
        .foregroundFill = Ui::GRAY400
    };

    return Ui::icon(i, 26) |
           Ui::center() |
           Ui::pinSize(46) |
           Ui::box(boxStyle);
}

export Ui::Child avatar() {
    return avatar(Mdi::ACCOUNT);
}

} // namespace Karm::Kira
