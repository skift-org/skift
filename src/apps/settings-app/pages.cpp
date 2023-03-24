#include <karm-ui/layout.h>
#include <karm-ui/scroll.h>

#include "app.h"

namespace Settings {

Ui::Child pageScafold(Ui::Child inner) {
    return inner |
           Ui::spacing(8) |
           Ui::maxSize({460, Ui::UNCONSTRAINED}) |
           Ui::grow() |
           Ui::hcenter() |
           Ui::vscroll() |
           Ui::grow();
}

} // namespace Settings
