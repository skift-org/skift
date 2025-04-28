module;

#include <karm-base/string.h>

export module Hideo.Canvas;

import Mdi;
import Karm.Ui;
import Karm.Kira;

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
