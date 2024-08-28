#include <hideo-base/scafold.h>

#include "app.h"

namespace Hideo::Peoples {

Ui::Child app() {
    return Hideo::scafold({
        .icon = Mdi::ACCOUNT,
        .title = "Peoples"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Peoples
