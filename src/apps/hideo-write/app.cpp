#include <hideo-base/scafold.h>

#include "app.h"

namespace Hideo::Write {

Ui::Child app() {
    return Hideo::scafold({
        .icon = Mdi::TEXT_BOX,
        .title = "Write"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Write
