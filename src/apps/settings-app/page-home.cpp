#include <karm-ui/input.h>
#include <karm-ui/layout.h>

#include "app.h"

namespace Settings {

Ui::Child tileButton(Mdi::Icon icon, String text) {
    return Ui::vflow(
               Ui::icon(icon, 36) | Ui::center() | Ui::grow(),
               Ui::text(text) | Ui::center()) |
           Ui::spacing(8) |
           Ui::bound() |
           Ui::minSize({96, 72}) |
           Ui::button(
               [](auto &) {
               },
               Ui::ButtonStyle::secondary());
}

Ui::Child pageHome(State const &) {
    return Ui::grid(
               Ui::GridStyle::simpleFixed({3, 96}, {3, 120}, 8),
               {
                   tileButton(Mdi::ACCOUNT, "Accounts"),
                   tileButton(Mdi::PALETTE, "Personalization"),
                   tileButton(Mdi::WIDGETS_OUTLINE, "Applications"),
                   tileButton(Mdi::LAPTOP, "System"),
                   tileButton(Mdi::WIFI, "Network"),
                   tileButton(Mdi::SECURITY, "Security & Privacy"),
                   tileButton(Mdi::UPDATE, "Updates"),
                   tileButton(Mdi::INFORMATION_OUTLINE, "About"),
               }) |
           Ui::spacing(8) | Ui::center() | Ui::bound() | Ui::grow();
}

} // namespace Settings
