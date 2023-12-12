#include <karm-ui/layout.h>

#include "navbar.h"

namespace Hideo {

Ui::Child navbar(Ui::Children children) {
    return Ui::vflow(
        Ui::separator(),
        Ui::hflow(
            4,
            children) |
            Ui::box({
                .padding = 8,
                .backgroundPaint = Ui::GRAY900,
            }));
}

Ui::Child navbarItem(Mdi::Icon icon, char const *text, bool selected, Ui::OnPress onPress) {
    return Ui::button(
               std::move(onPress),
               selected ? Ui::ButtonStyle::regular() : Ui::ButtonStyle::subtle(),
               Ui::vflow(
                   6,
                   Layout::Align::CENTER,
                   Ui::icon(icon),
                   Ui::labelMedium(text)) |
                   Ui::spacing(8)) |
           Ui::grow();
}

} // namespace Hideo
