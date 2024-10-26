#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>
#include <mdi/text-box.h>

#include "app.h"

namespace Hideo::Write {

Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::TEXT_BOX,
        .title = "Write"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Write
