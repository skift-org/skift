#include <karm-ui/layout.h>
#include <karm-ui/scroll.h>

#include "app.h"

namespace Hideo::Settings {

Ui::Child pageScafold(Ui::Child inner) {
    return inner |
           Ui::insets({0, 0, 0, 32}) |
           Ui::maxSize({460, Ui::UNCONSTRAINED}) |
           Ui::grow() |
           Ui::hcenter() |
           Ui::vscroll() |
           Ui::grow();
}

} // namespace Hideo::Settings
