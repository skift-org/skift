#include <hideo-base/scafold.h>

#include "app.h"

namespace Hideo::Apps {

Ui::Child app() {
    return Hideo::scafold({
        .icon = Mdi::BASKET,
        .title = "Apps"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Apps
