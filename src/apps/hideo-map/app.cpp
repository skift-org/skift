#include <hideo-base/scafold.h>

#include "app.h"

namespace Hideo::Map {

Ui::Child app() {
    return Hideo::scafold({
        .icon = Mdi::MAP,
        .title = "Map"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Map
