#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>

#include "app.h"

namespace Hideo::Tasks {

Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::VIEW_DASHBOARD,
        .title = "Tasks"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Tasks
