#include <hideo-base/scafold.h>

#include "game.h"

namespace Hideo::Chess {

Ui::Child game() {
    return Hideo::scafold({
        .icon = Mdi::CHESS_KING,
        .title = "Chess"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Chess
