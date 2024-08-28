#include <hideo-base/scafold.h>

#include "app.h"

namespace Hideo::Chat {

Ui::Child app() {
    return Hideo::scafold({
        .icon = Mdi::MESSAGE,
        .title = "Chat"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Chat
