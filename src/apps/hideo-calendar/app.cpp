export module Hideo.Calendar;

import Mdi;
import Karm.Core;
import Karm.Ui;
import Karm.Kira;

using namespace Karm;

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
