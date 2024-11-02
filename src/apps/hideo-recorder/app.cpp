#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>
#include <mdi/microphone.h>

#include "app.h"

namespace Hideo::Recorder {

Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::MICROPHONE,
        .title = "Recorder"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Recorder
