#include <karm-ui/layout.h>

#include "toolbar.h"

namespace Karm::Kira {

Ui::Child toolbar(Ui::Children children) {
    return Ui::vflow(
        Ui::hflow(4, children) |
            Ui::insets(8) |
            Ui::box({.backgroundFill = Ui::GRAY900}),
        Ui::separator()
    );
}

Ui::Child bottombar(Ui::Children children) {
    return Ui::vflow(
        Ui::separator(),
        Ui::hflow(4, children) |
            Ui::insets(8) |
            Ui::box({.backgroundFill = Ui::GRAY900})
    );
}

} // namespace Karm::Kira
