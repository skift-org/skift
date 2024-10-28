#include <karm-ui/layout.h>
#include <mdi/magnify.h>

#include "searchbar.h"

namespace Karm::Kira {

Ui::Child searchbar() {
    return Ui::hflow(
               0,
               Math::Align::VCENTER | Math::Align::START,
               Ui::text(Ui::TextStyles::labelMedium().withColor(Ui::GRAY400), "Search…") | Ui::grow(),
               Ui::icon(Mdi::MAGNIFY, 18)
           ) |
           Ui::insets({8, 12}) |
           Ui::box({
               .borderRadii = 4,
               .borderWidth = 1,
               .backgroundFill = Ui::GRAY900,
           }) |
           Ui::button(Ui::NOP);
}

} // namespace Karm::Kira
