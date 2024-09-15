#include <karm-kira/scaffold.h>
#include <karm-ui/layout.h>

#include "app.h"

namespace Hideo::Peoples {

Ui::Child app() {
    return Kr::scaffold({
        .icon = Mdi::ACCOUNT,
        .title = "Peoples"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Peoples
