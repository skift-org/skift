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
                .backgroundPaint = Ui::GRAY900,
            })
    );
}

Ui::Child navbarItem(Ui::OnPress onPress, Mdi::Icon icon, Str text, bool selected) {
    auto indicator = box(
        {
            .borderRadius = 99,
            .backgroundPaint = selected ? Ui::ACCENT600 : Gfx::ALPHA,
        },
        Ui::empty({16, 2})
    );

    return Ui::button(
               std::move(onPress),
               selected ? Ui::ButtonStyle::regular() : Ui::ButtonStyle::subtle(),
               Ui::vflow(
                   0,
                   Math::Align::CENTER,
                   Ui::icon(icon),
                   Ui::empty(4),
                   Ui::labelMedium(text),
                   indicator
               ) |
                   Ui::spacing({8, 10, 8, 6})
           ) |
           Ui::grow();
}

} // namespace Karm::Kira
