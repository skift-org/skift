#include <hideo-base/scafold.h>

#include "app.h"

namespace Hideo::Canvas {

Ui::Child app() {
    return Hideo::scafold({
        .icon = Mdi::DRAW,
        .title = "Canvas"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Canvas
