module;

#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>
#include <mdi/text-box.h>

export module Hideo.Write;

namespace Hideo::Write {

export Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::TEXT_BOX,
        .title = "Write"s,
        .body = [] {
            return Ui::empty();
        },
    });
}

} // namespace Hideo::Write
