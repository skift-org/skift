export module Hideo.Mines;

import Mdi;
import Karm.Core;
import Karm.Ui;
import Karm.Kira;

using namespace Karm;

namespace Hideo::Mines {

export Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::MINE,
        .title = "Mines"s,
        .body = [] {
            return Ui::empty();
        },
    });
}

} // namespace Hideo::Mines
