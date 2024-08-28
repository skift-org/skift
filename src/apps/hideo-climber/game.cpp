#include <hideo-base/scafold.h>

#include "game.h"

namespace Hideo::Climber {

Ui::Child game() {
    return Hideo::scafold({
        .icon = Mdi::IMAGE_FILTER_HDR,
        .title = "Climber"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Climber
