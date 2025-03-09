module;

#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>
#include <mdi/account.h>

export module Hideo.Peoples;

namespace Hideo::Peoples {

export Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::ACCOUNT,
        .title = "Peoples"s,
        .body = [] {
            return Ui::empty();
        },
    });
}

} // namespace Hideo::Peoples
