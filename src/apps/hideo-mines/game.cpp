#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>
#include <mdi/mine.h>

#include "game.h"

namespace Hideo::Mines {

Ui::Child game() {
    return Kr::scaffold({
        .icon = Mdi::MINE,
        .title = "Mines"s,
        .body = [] {
            return Ui::empty();
        },
    });
}

} // namespace Hideo::Mines
