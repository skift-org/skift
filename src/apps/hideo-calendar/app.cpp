module;

#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>
#include <mdi/calendar.h>

export module Hideo.Calendar;

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
