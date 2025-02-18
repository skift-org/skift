#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>
#include <mdi/calendar.h>

#include "app.h"

namespace Hideo::Calendar {

Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::CALENDAR,
        .title = "Calendar"s,
        .body = [] {
            return Ui::empty();
        },
    });
}

} // namespace Hideo::Calendar
