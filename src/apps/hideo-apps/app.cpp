#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>

#include "app.h"

namespace Hideo::Apps {

Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::BASKET,
        .title = "Apps"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Apps
