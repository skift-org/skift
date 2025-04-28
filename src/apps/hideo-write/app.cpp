module;

#include <karm-base/string.h>

import Mdi;
import Karm.Ui;
import Karm.Kira;

export module Hideo.Write;

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
