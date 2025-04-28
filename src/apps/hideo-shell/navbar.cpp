export module Hideo.Shell:navbar;

import Karm.Ui;
import Karm.Kira;
import :model;

using namespace Karm;

namespace Hideo::Shell {

export Ui::Child navbar(State const&) {
    return Kr::buttonHandle(
               Model::bind<Activate>(Panel::APPS)
           ) |
           Ui::slideIn(Ui::SlideFrom::BOTTOM);
}

} // namespace Hideo::Shell
