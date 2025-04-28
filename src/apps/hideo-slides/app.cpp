module;

#include <karm-base/string.h>

export module Hideo.Slides;

import Mdi;
import Karm.Kira;
import Karm.Ui;

namespace Hideo::Slides {

export Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::PRESENTATION,
        .title = "Slides"s,
        .body = [] {
            return Ui::empty();
        },
    });
}

} // namespace Hideo::Slides
