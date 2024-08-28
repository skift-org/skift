#include <hideo-base/scafold.h>

#include "game.h"

namespace Hideo::Mines {

Ui::Child game() {
    return Hideo::scafold({
        .icon = Mdi::MINE,
        .title = "Mines"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Mines
