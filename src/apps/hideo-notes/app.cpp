module;

#include <karm-base/string.h>

export module Hideo.Notes;

import Mdi;
import Karm.Ui;
import Karm.Kira;

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
