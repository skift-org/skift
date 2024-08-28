#include <hideo-base/scafold.h>

#include "app.h"

namespace Hideo::Slides {

Ui::Child app() {
    return Hideo::scafold({
        .icon = Mdi::PRESENTATION,
        .title = "Slides"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Slides
