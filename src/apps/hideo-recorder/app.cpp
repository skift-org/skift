export module Hideo.Recorder;

import Mdi;
import Karm.Core;
import Karm.Kira;
import Karm.Ui;

using namespace Karm;

namespace Hideo::Recorder {

export Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::MICROPHONE,
        .title = "Recorder"s,
        .body = [] {
            return Ui::empty();
        },
    });
}

} // namespace Hideo::Recorder
