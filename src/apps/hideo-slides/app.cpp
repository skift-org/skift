export module Hideo.Slides;

import Mdi;
import Karm.Kira;
import Karm.Ui;
import Karm.Core;

using namespace Karm;

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
