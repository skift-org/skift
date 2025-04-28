module;

#include <karm-base/string.h>

export module Hideo.Map;

import Mdi;
import Karm.Kira;
import Karm.Ui;

namespace Hideo::Map {

export Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::MAP,
        .title = "Map"s,
        .body = [] {
            return Ui::empty();
        },
    });
}

} // namespace Hideo::Map
