#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>
#include <mdi/image-filter-hdr.h>

#include "game.h"

namespace Hideo::Climber {

Ui::Child game() {
    return Kr::scaffold({
        .icon = Mdi::IMAGE_FILTER_HDR,
        .title = "Climber"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Climber
