export module Hideo.Map;

import Mdi;
import Karm.Core;
import Karm.Kira;
import Karm.Ui;

using namespace Karm;

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
