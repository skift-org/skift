#include <karm-ui/layout.h>

#include "navbar.h"

namespace Karm::Kira {

Ui::Child navbarContent(Ui::Children children) {
    return Ui::vflow(
        Ui::separator(),
        Ui::hflow(
            4,
            children
        ) |
            Ui::box({
                .padding = 8,
                .backgroundFill = Ui::GRAY900,
            })
    );
}

Ui::Child navbarItem(Ui::OnPress onPress, Mdi::Icon icon, Str text, bool selected) {
    return Ui::button(
               std::move(onPress),
               selected
                   ? Ui::ButtonStyle::regular().withForegroundFill(Ui::ACCENT500)
                   : Ui::ButtonStyle::subtle(),
               Ui::vflow(
                   0,
                   Math::Align::CENTER,
                   Ui::icon(icon),
                   Ui::empty(4),
                   Ui::labelSmall(text)
               ) |
                   Ui::insets({ 10, 8, 6, 8})
           ) |
           Ui::grow();
}

} // namespace Karm::Kira
