#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>
#include <mdi/message.h>

#include "app.h"

namespace Hideo::Chat {

Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::MESSAGE,
        .title = "Chat"s,
        .body = [] {
            return Ui::empty();
        },
    });
}

} // namespace Hideo::Chat
