module;

#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>
#include <mdi/code-braces.h>

export module Hideo.Code;

namespace Hideo::Code {

export Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::CODE_BRACES,
        .title = "Code"s,
        .body = [] {
            return Ui::empty();
        },
    });
}

} // namespace Hideo::Code
