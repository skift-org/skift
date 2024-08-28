#include <hideo-base/scafold.h>

#include "app.h"

namespace Hideo::Avplayer {

Ui::Child app() {
    return Hideo::scafold({
        .icon = Mdi::PLAY_CIRCLE,
        .title = "Media Player"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Avplayer
