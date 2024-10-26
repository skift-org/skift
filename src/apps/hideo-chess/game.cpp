#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>
#include <mdi/chess-king.h>

#include "game.h"

namespace Hideo::Chess {

Ui::Child game() {
    return Kr::scaffold({
        .icon = Mdi::CHESS_KING,
        .title = "Chess"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Chess
