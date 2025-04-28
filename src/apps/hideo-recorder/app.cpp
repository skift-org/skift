module;

#include <karm-base/string.h>

export module Hideo.Recorder;

import Mdi;
import Karm.Kira;
import Karm.Ui;

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
