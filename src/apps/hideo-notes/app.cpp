#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>
#include <mdi/format-list-bulleted.h>

#include "app.h"

namespace Hideo::Notes {

Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::FORMAT_LIST_BULLETED,
        .title = "Notes"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Notes
