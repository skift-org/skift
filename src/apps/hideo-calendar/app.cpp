module;

#include <karm-base/string.h>

export module Hideo.Calendar;

import Mdi;
import Karm.Ui;
import Karm.Kira;

namespace Hideo::Calendar {

export Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::CALENDAR,
        .title = "Calendar"s,
        .body = [] {
            return Ui::empty();
        },
    });
}

} // namespace Hideo::Calendar
