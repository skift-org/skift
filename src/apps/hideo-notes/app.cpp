module;

#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>
#include <mdi/format-list-bulleted.h>

export module Hideo.Notes;

namespace Hideo::Notes {

export Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::FORMAT_LIST_BULLETED,
        .title = "Notes"s,
        .body = [] {
            return Ui::empty();
        },
    });
}

} // namespace Hideo::Notes
