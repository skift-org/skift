export module Hideo.Canvas;

import Mdi;
import Karm.Core;
import Karm.Ui;
import Karm.Kira;

using namespace Karm;

namespace Hideo::Canvas {

export Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::DRAW,
        .title = "Canvas"s,
        .body = [] {
            return Ui::empty();
        },
    });
}

} // namespace Hideo::Canvas
