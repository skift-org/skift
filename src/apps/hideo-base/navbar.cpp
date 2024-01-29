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
    auto indicator = box(Ui::BoxStyle{
                             .borderRadius = 99,
                             .backgroundPaint = selected ? Ui::ACCENT600 : Gfx::ALPHA,
                         },
                         Ui::empty({16, 2}));

    return Ui::button(
               std::move(onPress),
               selected ? Ui::ButtonStyle::regular() : Ui::ButtonStyle::subtle(),
               Ui::vflow(
                   0,
                   Layout::Align::CENTER,
                   Ui::icon(icon),
                   Ui::empty(4),
                   Ui::labelMedium(text), indicator) |
                   Ui::spacing({8, 10, 8, 6})) |
           Ui::grow();
}

} // namespace Hideo
