export module Hideo.Peoples;

import Mdi;
import Karm.Core;
import Karm.Ui;
import Karm.Kira;

using namespace Karm;

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
