module;

import Mdi;
import Karm.Ui;
import Karm.Kira;
import Karm.Core;

export module Hideo.Write;

using namespace Karm;

namespace Hideo::Write {

export Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::TEXT_BOX,
        .title = "Write"s,
        .body = [] {
            return Ui::empty();
        },
    });
}

} // namespace Hideo::Write
