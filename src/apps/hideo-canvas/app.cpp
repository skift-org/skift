#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>
#include <mdi/draw.h>

#include "app.h"

namespace Hideo::Canvas {

Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::DRAW,
        .title = "Canvas"s,
        .body = [] {
            return Ui::empty();
        },
    });
}

} // namespace Hideo::Canvas
