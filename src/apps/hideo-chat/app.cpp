module;

#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>
#include <mdi/message.h>

export module Hideo.Chat;

namespace Hideo::Chat {

export Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::MESSAGE,
        .title = "Chat"s,
        .body = [] {
            return Ui::empty();
        },
    });
}

} // namespace Hideo::Chat
