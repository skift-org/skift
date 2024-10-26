#include <karm-ui/layout.h>
#include <mdi/close.h>

#include "side-panel.h"

namespace Karm::Kira {

Ui::Child sidePanelContent(Ui::Children children) {
    return Ui::vflow(children) |
           Ui::pinSize({320, Ui::UNCONSTRAINED});
}

Ui::Child sidePanelTitle(Ui::OnPress onClose, Str title) {
    return Ui::hflow(
               Ui::labelLarge(title),
               Ui::grow(NONE),
               Ui::button(
                   std::move(onClose),
                   Ui::ButtonStyle::subtle(),
                   Ui::icon(Mdi::CLOSE) | Ui::center()
               )
           ) |
           Ui::insets(6);
}

} // namespace Karm::Kira
