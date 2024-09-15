#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>

#include "game.h"

namespace Hideo::Mines {

Ui::Child game() {
    return Kr::scaffold({
        .icon = Mdi::MINE,
        .title = "Mines"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Mines
