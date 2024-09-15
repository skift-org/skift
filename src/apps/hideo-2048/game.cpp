#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>

#include "game.h"

namespace Hideo::H2048 {

Ui::Child game() {
    return Kr::scaffold({
        .icon = Mdi::GRID,
        .title = "2048"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::H2048
