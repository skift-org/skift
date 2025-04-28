module;

#include <karm-base/string.h>

export module Hideo.Mines;

import Mdi;
import Karm.Ui;
import Karm.Kira;

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
