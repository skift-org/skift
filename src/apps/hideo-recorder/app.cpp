#include <hideo-base/scafold.h>

#include "app.h"

namespace Hideo::Recorder {

Ui::Child app() {
    return Hideo::scafold({
        .icon = Mdi::MICROPHONE,
        .title = "Microphone"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Recorder
