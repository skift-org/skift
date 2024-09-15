#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>

#include "app.h"

namespace Hideo::Chat {

Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::MESSAGE,
        .title = "Chat"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Chat
