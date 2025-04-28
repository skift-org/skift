module;

#include <karm-base/string.h>

export module Hideo.Code;

import Mdi;
import Karm.Kira;
import Karm.Ui;

namespace Hideo::Code {

export Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::CODE_BRACES,
        .title = "Code"s,
        .body = [] {
            return Ui::empty();
        },
    });
}

} // namespace Hideo::Code
