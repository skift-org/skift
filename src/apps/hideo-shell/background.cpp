export module Hideo.Shell:background;

import Karm.Ui;
import :model;

using namespace Karm;

namespace Hideo::Shell {

export Ui::Child background(State const& state) {
    return Ui::image(state.background) |
           Ui::cover() |
           Ui::grow();
}

} // namespace Hideo::Shell
