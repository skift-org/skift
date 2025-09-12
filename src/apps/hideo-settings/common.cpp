export module Hideo.Settings:common;

import Karm.Ui;

using namespace Karm;

namespace Hideo::Settings {

export Ui::Child pageScaffold(Ui::Child inner) {
    return inner |
           Ui::insets({0, 0, 32, 0}) |
           Ui::maxSize({460, Ui::UNCONSTRAINED}) |
           Ui::grow() |
           Ui::hcenter() |
           Ui::vscroll() |
           Ui::grow();
}

} // namespace Hideo::Settings