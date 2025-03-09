module;

#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>
#include <mdi/microphone.h>

export module Hideo.Recorder;

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
