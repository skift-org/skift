module;

#include <karm-base/string.h>

export module Hideo.Peoples;

import Mdi;
import Karm.Ui;
import Karm.Kira;

namespace Hideo::Peoples {

export Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::ACCOUNT,
        .title = "Peoples"s,
        .body = [] {
            return Ui::empty();
        },
    });
}

} // namespace Hideo::Peoples
