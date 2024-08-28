#include <hideo-base/scafold.h>

#include "app.h"

namespace Hideo::Code {

Ui::Child app() {
    return Hideo::scafold({
        .icon = Mdi::CODE_BRACES,
        .title = "Code"s,
        .body = slot$(Ui::empty()),
    });
}

} // namespace Hideo::Code
