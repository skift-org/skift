#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>
#include <mdi/map.h>

#include "app.h"

namespace Hideo::Map {

Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::MAP,
        .title = "Map"s,
        .body = [] {
            return Ui::empty();
        },
    });
}

} // namespace Hideo::Map
