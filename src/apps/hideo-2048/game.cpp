#include <hideo-base/scafold.h>

#include "game.h"

namespace Hideo::H2048 {

Ui::Child game() {
    return Hideo::scafold({
        .icon = Mdi::GRID,
        .title = "2048"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::H2048
