export module Hideo.Notes;

import Mdi;
import Karm.Core;
import Karm.Ui;
import Karm.Kira;

using namespace Karm;

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
