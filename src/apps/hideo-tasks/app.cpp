#include <hideo-base/scafold.h>

#include "app.h"

namespace Hideo::Tasks {

Ui::Child app() {
    return Hideo::scafold({
        .icon = Mdi::VIEW_DASHBOARD,
        .title = "Tasks"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Tasks
