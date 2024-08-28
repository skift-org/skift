#include <hideo-base/scafold.h>

#include "app.h"

namespace Hideo::Notes {

Ui::Child app() {
    return Hideo::scafold({
        .icon = Mdi::FORMAT_LIST_BULLETED,
        .title = "Notes"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Notes
