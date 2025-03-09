module;

#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>
#include <mdi/mine.h>

export module Hideo.Mines;

namespace Hideo::Mines {

export Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::MINE,
        .title = "Mines"s,
        .body = [] {
            return Ui::empty();
        },
    });
}

} // namespace Hideo::Mines
