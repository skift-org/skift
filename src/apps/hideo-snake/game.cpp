#include <hideo-base/scafold.h>

#include "game.h"

namespace Hideo::Snake {

Ui::Child game() {
    return Hideo::scafold({
        .icon = Mdi::SNAKE,
        .title = "Snake"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Snake
