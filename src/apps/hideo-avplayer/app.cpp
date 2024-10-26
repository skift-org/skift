#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>
#include <mdi/play-circle.h>

#include "app.h"

namespace Hideo::Avplayer {

Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::PLAY_CIRCLE,
        .title = "Media Player"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Avplayer
