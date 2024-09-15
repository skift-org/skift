#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>

#include "app.h"

namespace Hideo::Slides {

Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::PRESENTATION,
        .title = "Slides"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Slides
