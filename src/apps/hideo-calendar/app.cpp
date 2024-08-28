#include <hideo-base/scafold.h>

#include "app.h"

namespace Hideo::Calendar {

Ui::Child app() {
    return Hideo::scafold({
        .icon = Mdi::CALENDAR,
        .title = "Calendar"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Calendar
