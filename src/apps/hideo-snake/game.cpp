#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>
#include <mdi/snake.h>

#include "game.h"

namespace Hideo::Snake {

Ui::Child game() {
    return Kr::scaffold({
        .icon = Mdi::SNAKE,
        .title = "Snake"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Snake
